# Shared Development Environments
Use this guide to conveniently set up reference development environments on almost any machine/platform.
Additional detailed guides are available in [`./docs`](./docs).

> ⚠️
<span style="color:orange">
Package versions and inherently their behaviour **may radically differ on individual platforms and architectures**.
Always refer to the reference platform and architecture for closest possible replication of the environment.
</span>


## Installing Nix
For more information about installing and using Nix, refer to https://nixos.org/download#nix-install. Supported systems are Linux, macOS, Windows (WSL2), and Docker.

For Windows (WSL2) and Linux use 
```shell
sh <(curl --proto '=https' --tlsv1.2 -L https://nixos.org/nix/install) --daemon
```

For macOS
```shell
sh <(curl --proto '=https' --tlsv1.2 -L https://nixos.org/nix/install)
```

## Enable flakes (global)
Enable the `nix-command` and `flakes` features system-wide:

```shell
sudo mkdir -p /etc/nix
printf "experimental-features = nix-command flakes\n" | sudo tee /etc/nix/nix.conf
```

## Starting development environment
This section describes how to use various language environments for the given courses.
The section assumes that Nix is installed on the system.
All language related tools should become available in the shell right after starting the corresponding development environment as described below.

The development environments can also be started from a locally cloned repository:
```shell
git clone https://git.fit.vutbr.cz/NESFIT/dev-envs.git
nix develop .#c
```

You can also modify the environment definitions (`c.nix`, `csharp.nix` etc.) to, e.g.,
- add JetBrains Rider, add `jetbrains.rider` to the `packages` list;
- add a particular C library for testing, add it to the `buildInputs` list.

Available packages can be found [here](https://search.nixos.org/packages?channel=23.11).

> ⚠️
<span style="color:orange">
Modified environments will not be taken into account when evaluating your submission unless the changes are committed in this repository.
Any local or external modifications outside this concrete repository are ignored during evaluation.
</span>

If you need a different language/environment, please open a PR adding a new devShell.
For contributor instructions, see [How to add a devShell](./nix/devShells/README.md).

### DevEnvs
> ℹ️
<span style="color:deepskyblue">
Always refer to the individual project requirements to see which tools are allowed.
</span>

The following language environments are available for IPK:

#### C/C++
```shell
nix develop --refresh "git+https://git.fit.vutbr.cz/NESFIT/dev-envs.git#c"
```

#### C#
```shell
nix develop --refresh "git+https://git.fit.vutbr.cz/NESFIT/dev-envs.git#csharp"
```

#### CLISP
```shell
nix develop --refresh "git+https://git.fit.vutbr.cz/NESFIT/dev-envs.git#clisp"
```

## VM Quick Start (QEMU)

From a local clone of this repository, run:

```shell
nix run .#run-vm-qemu
```

Without a local clone, you can run directly from the repository URL:

```shell
nix run --refresh "git+https://git.fit.vutbr.cz/NESFIT/dev-envs.git#run-vm-qemu"
nix run --refresh "git+https://git.fit.vutbr.cz/NESFIT/dev-envs.git#run-vm-qemu" -- --no-build --no-reset
```

Without a local clone (macOS), use the same command. The wrapper now passes flake reference through Docker build flow.

This command builds (or reuses) a QCOW image, prepares runtime artifacts in `.vm/`, and starts QEMU.

> ⚠️
<span style="color:orange">
By design, `run-vm-qemu` does **not persist guest disk or EFI changes by default**. Each run recreates the runtime overlay from the base image unless you explicitly pass `--no-reset`. If you want to keep existing runtime state without rebuilding the base image first, use `--no-build --no-reset` together.
</span>

### SSH login (NAT mode)

After boot, connect from another terminal:

```shell
ssh -p 2222 student@localhost
```

Default credentials:
- user: `student`
- password: `student4lab`

### Shutdown VM

Preferred (inside VM):

```shell
sudo shutdown -h now
```

If QEMU is running in foreground, you can also stop it with `Ctrl+C`.

For detailed VM usage and platform-specific options, see:
- [`./docs`](./docs)
- [`./docs/vm-build.md`](./docs/vm-build.md)
- [`./docs/vm-run-qemu.md`](./docs/vm-run-qemu.md)

## FAQ

### 1. Why does the environment behave differently on my machine than on the reference platform?

Package versions, transitive dependencies, and low-level system behavior can differ across Linux, macOS, WSL2, and CPU architectures. Always compare your setup with the course reference platform first, especially if a project requires exact tool behavior.

### 2. Why does `nix develop` or `nix run` say that flakes are not enabled?

Flakes are required by this repository. Enable `nix-command` and `flakes` in `/etc/nix/nix.conf` as described above, then start a new shell and try the command again.

### 3. Why does the QEMU VM lose my changes after restart?

That is the default behavior by design. `run-vm-qemu` recreates the runtime overlay on each run. If you want to keep VM disk and EFI state, use `--no-build --no-reset` with an already prepared image.

### 4. Why does `--no-reset` not pick up my newly rebuilt VM configuration?

Because `--no-reset` reuses the existing `.vm` overlay. If you rebuild the base image but keep the old runtime overlay, the VM will usually continue running on top of the older backing image. To apply new base-image changes, run without `--no-reset` or remove `.vm/`.

### 5. Why is the VM slower or networking different on macOS or WSL2?

Acceleration and networking depend on the host platform. Linux can use KVM, macOS uses HVF, and WSL2 commonly falls back to TCG. Bridge mode support also differs by platform, so NAT mode is usually the safest default.

### 6. What should I clean up if something is weirdly broken?

Start with the local runtime state. Remove `.vm/` to discard cached VM overlay and firmware state, then rerun the VM command. If the problem still persists, run `nix-collect-garbage -d` to remove old unreferenced store paths and rebuild again. This is often enough to fix stale local state after interrupted runs or older image revisions.

### 7. When should I reinstall Nix completely?

If even simple commands such as `nix --version`, `nix flake show`, or `nix develop` fail in unrelated directories, the problem may be your Nix installation rather than this repository. In that case, a clean reinstall of Nix is a reasonable last step after trying local cleanup. After reinstalling, re-enable flakes and rerun the commands from this README.

### 8. Why does the VM have broken internet connectivity, and how do I check IP forwarding?

On Linux and WSL2, check it with `sysctl net.ipv4.ip_forward` or `cat /proc/sys/net/ipv4/ip_forward`. If it returns `0`, enable it temporarily with `sudo sysctl -w net.ipv4.ip_forward=1`, or persist it by adding `net.ipv4.ip_forward=1` to your sysctl configuration. Missing IP forwarding can break host-level routing scenarios, NAT-mode internet access from the guest, and bridge-based networking.

## Cleanup

Remove local VM runtime artifacts:

```shell
rm -rf .vm
```

Remove old/unreferenced Nix store paths:

```shell
nix-collect-garbage -d
```
