# pinentry-hypr

A Hyprland-native GnuPG pinentry, built with **Qt6**, **QML**, and **hyprutils**.

This project provides a standalone `pinentry` dialog that is visually and behaviorally identical to the official `hyprpolkitagent`. It integrates seamlessly into the Hyprland ecosystem, supporting Wayland-native window rules, the `org.hyprland.style` plugin, and automatic floating/centering.

![Screenshot](https://raw.githubusercontent.com/hyprwm/hyprpolkitagent/main/assets/screenshot.png)
*(Visuals match the official hyprpolkitagent shown above)*

## Features

- **Hyprland Native:** Built using the same stack as official Hyprland utilities (Qt6/QML/hyprutils).
- **Visual Consistency:** Matches the colors, borders, and layout of `hyprpolkitagent`.
- **Secrets API Integration:** Full support for `libsecret` (GNOME Keyring, etc.) for persistent password caching, mirroring `pinentry-qt`.
- **Wayland Native:** Handles window properties correctly for tiling and floating in Hyprland.
- **Passphrase Visibility:** Integrated "Show" toggle to verify input.

## Dependencies

- `qt6-base`
- `qt6-declarative`
- `qt6-quickcontrols2`
- `hyprutils`
- `hyprland-qt-support`
- `libsecret`

## Installation

### 1. Build from source
```bash
mkdir build && cd build
cmake ..
make
```

### 2. Configure GnuPG
Edit your `~/.gnupg/gpg-agent.conf` to use the new binary:
```conf
pinentry-program /path/to/pinentry-hypr/build/pinentry-hypr
```

### 3. Reload GPG Agent
```bash
gpgconf --kill gpg-agent
```

## Hyprland Configuration (Optional)

While `pinentry-hypr` is designed to be recognized as a dialog, you can ensure it always floats and stays centered by adding these rules to your `hyprland.conf`:

```ini
windowrulev2 = float, class:^(pinentry-hypr)$
windowrulev2 = center, class:^(pinentry-hypr)$
windowrulev2 = pin, class:^(pinentry-hypr)$
windowrulev2 = stayfocused, class:^(pinentry-hypr)$
```

## License

BSD-3-Clause
