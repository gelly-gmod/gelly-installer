# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.5.0] - 2025-02-14

### Added

- Added a completely new UI.
- Switched from ImGui + raylib to Clay + SDL2.

## [1.4.0] - 2025-01-04

### Added

- Added a protocol handler, so you can now open the installer by opening `gelly-installer://` links.
- Auto-update is now available by passing `autoupdate` as a parameter to the installer.
- [![Static Badge](https://img.shields.io/badge/Install_with-Gelly_Installer-red?style=flat&link=https%3A%2F%2Fintradeus.github.io%2Fhttp-protocol-redirector%2F%3Fr%3Dgelly-installer%3Aautoupdate)](https://intradeus.github.io/http-protocol-redirector/?r=gelly-installer:autoupdate)

## [1.3.0] - 2025-01-03

### Added

- The installer now automatically relocates itself to the installed apps directory.
- This is in preparation for the upcoming auto-update feature.
- Note: Opening the installer again will launch the installed app and not the one that was previously launched.

## [1.2.0] - 2024-12-27

### Added

- Added logging to better troubleshoot issues.

## [1.1.0] - 2024-12-26

### Changed

- Updated the UI to be more user-friendly.
- Buttons take up more space now.

## [1.0.0] - 2024-12-24

### Added

- Initial release.