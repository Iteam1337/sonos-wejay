# Changelog

## [2.2.1] - 2018-11-22

### Changed

- Use multi stage Docker build, reduces image size from ~1.4GB to ~500MB #15

## [2.2.0] - 2018-11-22

### Added

- Add Spotify copy/paste #17

## [2.1.1] - 2018-11-10

### Changed

- Revert async/await. Can't build in Docker.

## [2.1.0] - 2018-11-09

### Feat

- Added time

## [2.0.0] - 2018-11-09

### Changed

- Switched to async/await

## [1.0.1] - 2018-11-04

### Fixed

- Fix typo in emoji-response for top list and most played when there is no result

## [1.0.0] - 2018-11-04

Feels pretty solid and production worthy, let's bump it to 1.0.0! :tada:

### Changed

- Major refactoring

## [0.8.3] - 2018-11-03

### Changed

- Use track information from database instead of querying Spotify for each track

## [0.8.2] - 2018-11-03

### Changed

- Add track information to database insert

## [0.8.1] - 2018-11-02

### Fixed

- Fixed blame for music added by library

## [0.8.0] - 2018-11-02

### Added

- Most played track
- Added `fullqueue`

## [0.7.2] - 2018-11-02

### Fixed

- Search command for mention

## [0.7.1] - 2018-11-02

### Fixed

- Handle params for app mentions

## [0.7.0] - 2018-11-02

### Added

- Add Slack event `app_mention` to handle direct message in other channels

## [0.6.3] - 2018-11-02

### Fixed

- Add database entry to `queueAsNext` command

## [0.6.2] - 2018-11-01

### Fixed

- All commands that queue adds a database entry
- Empty search results (#6)

## [0.6.1] - 2018-11-01

### Fixed

- DB hostname

## [0.6.0] - 2018-11-01

### Added

- Add toplist

## [0.5.0] - 2018-10-26

### Added

- Play track with number
- Emoji support (:+1:, :-1:)

### Changed

- Refactored

### Fixed

- Added `blame` to help
- Fix `blame` when track has been added using Sonos app and when multiple people have added the same track.

## [0.4.0] - 2018-10-23

### Added

- Beta version of blame functionality
