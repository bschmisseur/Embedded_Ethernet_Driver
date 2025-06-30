/**
 * @file video_codec.cpp
 * 
 * @brief Implementation of public and private interfaces for the video codec
 * 
 * @author Bryce Schmisseur
 * 
 */

// Header Includes
#include "video_codec.hpp"

// Standard Incudes
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}
#include <iostream>

// Project Includes


namespace EthernetDriverSimulation {

    ErrorCode VideoCodec::encodeGifToH264(const std::string& gifPath, const std::string& h264Path) {
        
        AVFormatContext *inputContext = nullptr;
        if (avformat_open_input(&inputContext, gifPath.c_str(), nullptr, nullptr) != 0) {
            return ErrorCode::ENCODE_CANNOT_OPEN_FILE;
        }

        if (avformat_find_stream_info(inputContext, nullptr) < 0) {
            avformat_close_input(&inputContext);
            return ErrorCode::ENCODE_FAILED_STREAM_INFO;
        }

        size_t videoStreamIndex = -1;
        for (size_t i = 0; i < inputContext->nb_streams; ++i) {
            if (inputContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
                videoStreamIndex = i;
                break;
            }
        }

        if (videoStreamIndex == -1) {
            avformat_close_input(&inputContext);
            return ErrorCode::ENCODE_NO_VIDEO_STREAM;
        }

        AVCodecParameters* inputCodecParams = inputContext->streams[videoStreamIndex]->codecpar;
        const AVCodec* decoder = avcodec_find_decoder(inputCodecParams->codec_id);
        AVCodecContext* decoderContext = avcodec_alloc_context3(decoder);
        avcodec_parameters_to_context(decoderContext, inputCodecParams);
        avcodec_open2(decoderContext, decoder, nullptr);

        AVFormatContext* outputContext = nullptr;
        avformat_alloc_output_context2(&outputContext, nullptr, nullptr, h264Path.c_str());
        if (!outputContext) {
            return ErrorCode::ENCODE_OUTPUT_ALLOC_FAIL;
        }

        const AVCodec* encoder = avcodec_find_encoder(AV_CODEC_ID_H264);
        if (!encoder) {
            return ErrorCode::ENCODE_ENCODER_NOT_FOUND;
        }

        AVStream* outputStream = avformat_new_stream(outputContext, nullptr);
        AVCodecContext* encoderContext = avcodec_alloc_context3(encoder);

        encoderContext->width     = decoderContext->width;
        encoderContext->height    = decoderContext->height;
        encoderContext->pix_fmt   = AV_PIX_FMT_YUV420P;
        encoderContext->time_base = AVRational{1, 25};
        encoderContext->framerate = AVRational{25, 1};
        encoderContext->bit_rate  = 400000;

        if (outputContext->oformat->flags & AVFMT_GLOBALHEADER) {
            encoderContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
        }

        if (avcodec_open2(encoderContext, encoder, nullptr) < 0) {
            return ErrorCode::ENCODE_ENCODER_OPEN_FAIL;
        }

        avcodec_parameters_from_context(outputStream->codecpar, encoderContext);
        outputStream->time_base = encoderContext->time_base;

        if (!(outputContext->oformat->flags & AVFMT_NOFILE)) {
            if (avio_open(&outputContext->pb, h264Path.c_str(), AVIO_FLAG_WRITE) < 0) {
                return ErrorCode::ENCODE_CANNOT_OPEN_OUTPUT;
            }
        }

        if (avformat_write_header(outputContext, nullptr) < 0) {
            return ErrorCode::ENCODE_HEADER_WRITE_FAIL;
        }

        SwsContext* scaleContext = sws_getContext(
            decoderContext->width, decoderContext->height, decoderContext->pix_fmt,
            encoderContext->width, encoderContext->height, encoderContext->pix_fmt,
            SWS_BILINEAR, nullptr, nullptr, nullptr
        );

        AVPacket* packet      = av_packet_alloc();
        AVFrame* frame        = av_frame_alloc();
        AVFrame* converted    = av_frame_alloc();
        converted->format     = encoderContext->pix_fmt;
        converted->width      = encoderContext->width;
        converted->height     = encoderContext->height;
        av_image_alloc(converted->data, converted->linesize, encoderContext->width, encoderContext->height, encoderContext->pix_fmt, 32);

        int frameCounter = 0;

        while (av_read_frame(inputContext, packet) >= 0) {
            if (packet->stream_index != videoStreamIndex) {
                av_packet_unref(packet);
                continue;
            }

            if (avcodec_send_packet(decoderContext, packet) == 0) {
                while (avcodec_receive_frame(decoderContext, frame) == 0) {
                    sws_scale(scaleContext, frame->data, frame->linesize, 0, decoderContext->height,
                            converted->data, converted->linesize);

                    converted->pts = frameCounter++;

                    if (avcodec_send_frame(encoderContext, converted) == 0) {
                        AVPacket* outPacket = av_packet_alloc();
                        while (avcodec_receive_packet(encoderContext, outPacket) == 0) {
                            outPacket->stream_index = outputStream->index;
                            av_interleaved_write_frame(outputContext, outPacket);
                            av_packet_unref(outPacket);
                        }
                        av_packet_free(&outPacket);
                    }
                }
            }

            av_packet_unref(packet);
        }

        // Final flush
        avcodec_send_frame(encoderContext, nullptr);
        AVPacket* flushPacket = av_packet_alloc();
        while (avcodec_receive_packet(encoderContext, flushPacket) == 0) {
            flushPacket->stream_index = outputStream->index;
            av_interleaved_write_frame(outputContext, flushPacket);
            av_packet_unref(flushPacket);
        }
        av_packet_free(&flushPacket);

        av_write_trailer(outputContext);

        // Cleanup
        av_frame_free(&frame);
        av_frame_free(&converted);
        av_packet_free(&packet);
        sws_freeContext(scaleContext);
        avcodec_free_context(&decoderContext);
        avcodec_free_context(&encoderContext);
        avformat_close_input(&inputContext);
        if (!(outputContext->oformat->flags & AVFMT_NOFILE)) {
            avio_closep(&outputContext->pb);
        }
        avformat_free_context(outputContext);

        return ErrorCode::SUCCESS;
    }

    ErrorCode VideoCodec::decodeH264ToGif(const std::string& h264Path, const std::string& gifPath) {
        AVFormatContext* inputContext = nullptr;
        if (avformat_open_input(&inputContext, h264Path.c_str(), nullptr, nullptr) < 0) {
            return ErrorCode::DECODE_CANNOT_OPEN_FILE;
        }

        if (avformat_find_stream_info(inputContext, nullptr) < 0) {
            avformat_close_input(&inputContext);
            return ErrorCode::DECODE_FAILED_STREAM_INFO;
        }

        size_t videoStreamIndex = -1;
        for (size_t i = 0; i < inputContext->nb_streams; ++i) {
            if (inputContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
                videoStreamIndex = i;
                break;
            }
        }

        if (videoStreamIndex == -1) {
            avformat_close_input(&inputContext);
            return ErrorCode::DECODE_NO_VIDEO_STREAM;
        }

        AVCodecParameters* inputCodecParams = inputContext->streams[videoStreamIndex]->codecpar;
        const AVCodec* decoder = avcodec_find_decoder(inputCodecParams->codec_id);
        AVCodecContext* decoderContext = avcodec_alloc_context3(decoder);
        avcodec_parameters_to_context(decoderContext, inputCodecParams);
        avcodec_open2(decoderContext, decoder, nullptr);

        AVFormatContext* outputContext = nullptr;
        avformat_alloc_output_context2(&outputContext, nullptr, nullptr, gifPath.c_str());
        if (!outputContext) {
            return ErrorCode::DECODE_OUTPUT_ALLOC_FAIL;
        }

        const AVCodec* gifEncoder = avcodec_find_encoder(AV_CODEC_ID_GIF);
        if (!gifEncoder) {
            return ErrorCode::DECODE_ENCODER_NOT_FOUND;
        }

        AVStream* outputStream = avformat_new_stream(outputContext, gifEncoder);
        AVCodecContext* gifContext = avcodec_alloc_context3(gifEncoder);

        gifContext->height    = decoderContext->height;
        gifContext->width     = decoderContext->width;
        gifContext->pix_fmt   = AV_PIX_FMT_RGB8;
        gifContext->time_base = AVRational{1, 10};  // 10 FPS

        if (outputContext->oformat->flags & AVFMT_GLOBALHEADER)
            gifContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

        if (avcodec_open2(gifContext, gifEncoder, nullptr) < 0) {
            return ErrorCode::DECODE_ENCODER_OPEN_FAIL;
        }

        avcodec_parameters_from_context(outputStream->codecpar, gifContext);
        outputStream->time_base = gifContext->time_base;

        if (!(outputContext->oformat->flags & AVFMT_NOFILE)) {
            if (avio_open(&outputContext->pb, gifPath.c_str(), AVIO_FLAG_WRITE) < 0) {
                return ErrorCode::DECODE_CANNOT_OPEN_OUTPUT;
            }
        }

        if (avformat_write_header(outputContext, nullptr) < 0) {
            avcodec_free_context(&decoderContext);
            avcodec_free_context(&gifContext);
            avformat_close_input(&inputContext);
            if (!(outputContext->oformat->flags & AVFMT_NOFILE)) {
                avio_closep(&outputContext->pb);
            }
            avformat_free_context(outputContext);
            return ErrorCode::DECODE_HEADER_WRITE_FAIL;
        }

        SwsContext* scaleContext = sws_getContext(
            decoderContext->width, decoderContext->height, decoderContext->pix_fmt,
            gifContext->width, gifContext->height, gifContext->pix_fmt,
            SWS_BILINEAR, nullptr, nullptr, nullptr
        );

        AVPacket* packet     = av_packet_alloc();
        AVFrame* rawFrame    = av_frame_alloc();
        AVFrame* rgbFrame    = av_frame_alloc();
        rgbFrame->format     = gifContext->pix_fmt;
        rgbFrame->width      = gifContext->width;
        rgbFrame->height     = gifContext->height;
        av_image_alloc(rgbFrame->data, rgbFrame->linesize, gifContext->width, gifContext->height, gifContext->pix_fmt, 32);

        int64_t framePts = 0;

        while (av_read_frame(inputContext, packet) >= 0) {
            if (packet->stream_index != videoStreamIndex) {
                av_packet_unref(packet);
                continue;
            }

            if (avcodec_send_packet(decoderContext, packet) == 0) {
                while (avcodec_receive_frame(decoderContext, rawFrame) == 0) {
                    sws_scale(
                        scaleContext,
                        rawFrame->data, rawFrame->linesize,
                        0, decoderContext->height,
                        rgbFrame->data, rgbFrame->linesize
                    );

                    rgbFrame->pts = framePts++;

                    if (avcodec_send_frame(gifContext, rgbFrame) == 0) {
                        AVPacket* gifPacket = av_packet_alloc();
                        while (avcodec_receive_packet(gifContext, gifPacket) == 0) {
                            gifPacket->stream_index = outputStream->index;
                            av_interleaved_write_frame(outputContext, gifPacket);
                            av_packet_unref(gifPacket);
                        }
                        av_packet_free(&gifPacket);
                    }
                }
            }

            av_packet_unref(packet);
        }

        // Flush encoder
        avcodec_send_frame(gifContext, nullptr);
        AVPacket* finalPacket = av_packet_alloc();
        while (avcodec_receive_packet(gifContext, finalPacket) == 0) {
            finalPacket->stream_index = outputStream->index;
            av_interleaved_write_frame(outputContext, finalPacket);
            av_packet_unref(finalPacket);
        }
        av_packet_free(&finalPacket);

        av_write_trailer(outputContext);

        // Cleanup
        av_frame_free(&rawFrame);
        av_frame_free(&rgbFrame);
        av_packet_free(&packet);
        sws_freeContext(scaleContext);
        avcodec_free_context(&decoderContext);
        avcodec_free_context(&gifContext);
        avformat_close_input(&inputContext);
        if (!(outputContext->oformat->flags & AVFMT_NOFILE)) {
            avio_closep(&outputContext->pb);
        }
        avformat_free_context(outputContext);

        return ErrorCode::SUCCESS;
    }
}