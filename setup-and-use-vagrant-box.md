# OS Challenge Project Setup Guide

This guide will walk you through setting up the virtualized development environment and configuring the CLion IDE for remote compilation and debugging.

## 1\. Local Environment Setup

First, you need to get the official project Virtual Machines (VMs) running on your computer. These VMs provide the standardized environment where your server will be tested.

### How to Set Up the VMs

Here are the steps to get the client and server VMs running, based on the project document.

1.  **Install Prerequisites**:
    You need to install **VirtualBox** and **Vagrant** on your computer.

    * [VirtualBox Download Page](https://www.virtualbox.org/wiki/Downloads)
    * [Vagrant Download Page](https://www.vagrantup.com/downloads)

2.  **Clone the Common Repository**:
    The configuration for the VMs is in the `os-challenge-common` repository. Clone it to your machine:

    ```bash
    git clone https://github.com/dtu-ese/os-challenge-common.git
    ```

3.  **Navigate and Add the Vagrant Box**:
    Go into the directory for the correct architecture (`x86_64` for most computers) and add the specified Ubuntu "box," which is a base image for a VM.
    configuring the CLion IDE for remote comp
    ```bash
    cd os-challenge-common/x86_64
    vagrant box add ubuntu/focal64
    ```

4.  **Start the VMs**:
    This command reads the `Vagrantfile` in the directory and creates, configures, and starts both the `server` and `client` VMs.

    ```bash
    vagrant up
    ```

    * **Troubleshooting**: If you encounter an error about IP address ranges, refer to the FAQ section 6.14 in the project document. You may need to create a file at `/etc/vbox/networks.conf` with the content `* 192.168.0.0/16` (look it up).

-----

## 2\. CLion Remote Development Configuration

This setup allows you to write code in CLion on your local machine while compiling, running, and debugging it directly on the `server` VM.

### Step 1: Install Build Tools on the VM

The base VM is minimal and does not include a compiler or build tools. You must install them.

1.  SSH into the running server VM:
    ```bash
    # Run this from the os-challenge-common/x86_64 directory
    vagrant ssh server
    ```
2.  Inside the VM, run the following command to install `cmake`, `gcc`, `make`, and `gdb`:
    ```bash
    sudo apt-get update && sudo apt-get install -y cmake build-essential gdb
    ```

### Step 2: Get SSH Credentials

CLion needs to know how to connect to the VM.

1.  On your **local machine** (not in the VM), navigate to the `os-challenge-common/x86_64` directory.
2.  Run the following command to display the SSH configuration:
    ```bash
    vagrant ssh-config server
    ```
3.  Keep this output handy. You will need the values for `HostName`, `Port`, `User`, and `IdentityFile`.

### Step 3: Configure CLion

1.  **Add Remote Toolchain**:

    * In CLion, go to `File > Settings > Build, Execution, Deployment > Toolchains`.
    * Click `+` and select `Remote Host`.
    * Fill in the details from the `vagrant ssh-config` output:
        * **Host**: `127.0.0.1` (from `HostName`)
        * **Port**: `2222` (or whatever `Port` is listed)
        * **User name**: `vagrant` (from `User`)
        * **Authentication type**: Select `Key pair`.
        * **Private key file**: Copy the full path from the `IdentityFile` line.
    * Click **Test Connection**. Once it succeeds, click **OK**. CLion will automatically detect the build tools you installed on the VM.

2.  **Set the Remote Toolchain for Your Project**:

    * Go to `File > Settings > Build, Execution,Deployment > CMake`.
    * Select your "Debug" profile. In the **Toolchain** dropdown, select the `Remote Host` toolchain you just created.
      * You might need to change the name as it sometimes adds extra, so just keep it as `cmake-build-debug`
    * Click **Apply** and **OK**.

3.  **Reload the Project**:

    * If CLion prompts you to reload, do so. Otherwise, force a reload by going to `Tools > CMake > Reload CMake Project`.
    * If you encounter an error about `CMakeCache.txt`, delete the `cmake-build-debug` directory from your project and reload again.

-----

## 3\. Development Workflow

Your environment is now fully configured.

* **Edit Code**: Write and edit your code in the CLion window on your local machine.
* **Build/Run/Debug**: Use the standard **Run `▶️`**, **Debug `🐞`**, and **Build `🔨`** buttons in CLion.
* **Execution**: CLion will automatically sync your files to the VM, compile the code there, and run the executable there. All program output will be streamed back to the CLion console.
* **Testing**: Keep a separate terminal open and logged into the client VM (`vagrant ssh client`) to run the test scripts (e.g., `run-client.sh`) against your server.