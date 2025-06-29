# Ethernet Driver Simulator

A lightweight, cross‑platform simulation that models an embedded‑device Ethernet stack, complete with command / response messaging and an H.264 video‑streaming use‑case. The project ships with pre‑built binaries for convenience, but you can also build from source on both Windows and Linux.

---

## Quick Start

### 1 ▪ Running the Binaries
Pre‑compiled executables for **Windows** and **Linux** reside in **`release/bin/`**:

| OS | Binary |
|-----|---------|
| Windows | `./ethernet_driver_simulator.exe` |
| Linux   | `./ethernet_driver_simulator`     |

Using the Make Target `run` as shown below will determine the platform currently being used and execute the respective binary.

> From the repository root run:
> ```bash
> make run
> ```

You will be greeted by an interactive command‑line menu (see *Test Cases* below).

### 2 ▪ Building the Application

> From the repository root run:
> ```bash
> make all      # compile, build, and distribute
> ```

Then follow the instructions above to run the application.

#### Windows prerequisites
| Package | Version (tested) | Notes |
|---------|------------------|-------|
| MinGW‑w64 | ≥ 13.2.0 | Provides GCC and standard build tools |
| GNU Make  | ≥ 4.4.1  | `mingw32-make` is acceptable |

Install via **MSYS2**, **Chocolatey**, or direct downloads.

#### Linux prerequisites
Install the following packages via your distro’s package manager:

```bash
sudo apt install build-essential g++ pkg-config \
     libavcodec-dev libavformat-dev libavutil-dev \
     libswscale-dev libavdevice-dev
```
*(Package names shown for Debian/Ubuntu; adjust as required for other distros.)*

---

## Test Cases & Walk‑through
After launching the application you will see a numbered menu with three core demonstrations. Choose the option that matches the scenario you wish to explore.

### ① Perform Handshake
*Demonstrates a standard request / response exchange between **host** and **device**.*

1. The host constructs an Ethernet frame containing a *handshake* command.
2. The device validates the frame and returns an acknowledgement packet.
3. The host prints:
   * **TX Frame** – raw bytes sent
   * **RX Frame** – acknowledgement received

### ② Injected Errors
*Exercises the protocol’s robustness by deliberately corrupting outgoing frames.*

> You will be prompted to choose an error type:
>
> * **Malformed Frame** – extra header bytes / bad delimiter.
> * **Incorrect EtherType** – type field set to an unsupported value.

For each case the device replies with an error packet bearing code **`0xFAFAFA`**. The host prints both the malformed TX frame and the returned error frame.

### ③ Request Video Capture
*Full end‑to‑end simulation of a streaming video.*

1. Host issues a **capture‑video** request.
2. Device “records” a short clip, encodes it with H.264, fragments it into Ethernet frames, and streams them back.
3. Host reassembles the stream, decodes it, and stores the resulting GIF under `output/video.gif` (path printed on completion).

---

## Further Reading
* **Request for Discussion (RFD)** – detailed design rationale and architecture can be found [📄 here]((https://github.com/bschmisseur/Ethernet_Driver_Simulator/wiki/Request-for-Discussion)).
