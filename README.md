# Pine Virtual Machine

Think of it like JVM but with as little abstraction as possible like C. The Pine VM creates a runtime environment for its own bytecode. This program provides computer users a second layer to their system. Our goal is to create a universal bytecode virtual machine that is highly flexible and offers maximum performance.

## Getting Started

These following instructions will guide you on how to install Pine Virtual Machine on your local system for development as well as testing. Be sure to have a look at the table below for currently supported platforms.

<table>
    <thead>
        <tr>
            <th>Operating System</th>
            <th>Architecture</th>
            <th>Status</th>
            <th>Note</th>
        </tr>
    </thread>
    <tbody>
        <tr>
            <td align="center">MacOS 10.13+</td>
            <td align="center">64-bit</td>
            <td align="center">&#10003;</td>
            <td align="center">Official</td>
        </tr>
        <tr>
            <td rowspan=2 align="center">Windows 10+</td>
            <td align="center">64-bit</td>
            <td></td>
            <td align="center">Planned</td>
        </tr>
        <tr>
            <td align="center">32-bit</td>
            <td></td>
            <td align="center">Planned</td>
        </tr>
        <tr>
            <td rowspan=2 align="center">Generic Linux</td>
            <td align="center">64-bit</td>
            <td></td>
            <td align="center">TBD</td>
        </tr>
        <tr>
            <td align="center">32-bit</td>
            <td></td>
            <td align="center">TBD</td>
        </tr>
    </tbody>
</table>

## Build Instructions

### Prerequisites

Before installing the project, please have the following programs installed:
* git
* gcc
* GNU make

### Installing

Once the project is downloaded, run `make` and follow the instructions printed. This command builds the executable and install an alias on `/usr/local/bin`.

### Uninstalling

To uninstall, simply run `make uninstall` to delete the executable's alias on `/usr/local/bin`.

## Deployment

These instructions will guide you on how to deploy Pine Virtual Machine on your local system. Important and helpful information will be noted down below. Be sure to have a look at our wiki page for more details including the VM's complete documentation.

### Running the VM

Run `pvm` to see the various options and arguments to properly run the VM. Make sure the program is installed properly. For quick bytecode execution, simply run `pvm [file]`.

## Notable Changes

Please read CHANGELOG.md for information regarding detailed changes throughout the project's lifetime.
