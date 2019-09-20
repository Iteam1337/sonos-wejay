# [6.3.0](https://github.com/Iteam1337/sonos-wejay/compare/v6.2.0...v6.3.0) (2019-09-20)


### Features

* start using decco for json ([#56](https://github.com/Iteam1337/sonos-wejay/issues/56)) ([4e364b5](https://github.com/Iteam1337/sonos-wejay/commit/4e364b5))

# [6.2.0](https://github.com/Iteam1337/sonos-wejay/compare/v6.1.0...v6.2.0) (2019-09-20)


### Bug Fixes

* update date-fns bindings ([e4b1e85](https://github.com/Iteam1337/sonos-wejay/commit/e4b1e85))


### Features

* check if track exists before queuing up a new track ([#54](https://github.com/Iteam1337/sonos-wejay/issues/54)) ([ccc2719](https://github.com/Iteam1337/sonos-wejay/commit/ccc2719))

## [6.1.0] - 2019-05-23

### Changed

- Added an easter egg update

## [6.0.1] - 2019-04-16

### Fixed

- Parse search string with `encodeURIComponent`

## [6.0.0] - 2019-04-12

**BREAKING CHANGES**: Remove `library` search command, because no one uses it.

### Changed

- Remove `library` command
- Refactored Spotify search and track using
  [@wejay/spotify](https://github.com/Iteam1337/wejay-utils) utility.
- Lots of clean up using `[@bs.obj]`, combined switches and more

### Fixed

- Out of bounds issue in spotify search #44

## [5.0.1] - 2019-03-29

### Fixed

- Only log to Elasticsearch when we actually have a user #43

## [5.0.0] - 2019-03-22

**BREAKING CHANGES**: Move all Elasticsearch queries to external service. API
remains the same for Wejay, but you'll have to the data yourself.

## [4.0.0] - 2019-03-08

**BREAKING CHANGES**: Removed MySQL, only use Elasticsearch.

### Changed

- Major rewrite to return replies and handle them at the top level. This will
  make other integrations waaay easier as we can use the service as an API as
  well.

## [3.4.4] - 2019-02-22

### Fixed

- Fix bug with friday easter egg

## [3.4.3] - 2019-02-17

### Fixed

- Handle skin tone emojis #41

## [3.4.2] - 2019-02-12

### Changed

- Refactor a bunch of types and created more concise modules
- Use built in json parse
- Use new line helper

### Fixed

- Add command from unknown command logging to `args` instead of the command string. Because we'll get a bunch of unique command values otherwise.

## [3.4.1] - 2019-02-12

### Fixed

- Handle end of current queue #40

## [3.4.0] - 2019-01-11

### Added

- New easter egg

### Changed

- Updated styling of search results

## [3.3.0] - 2019-01-04

### Added

- Welcome message at `/`
- Ability to paste a Spotify playlist link and queue the entire playlist

## Changed

- Remove `OK` status code when queuing a track from search results. Return the message here instead.

## [3.2.0] - 2018-12-21

### Added

- Helpful messages when the queue is empty #38

## [3.1.1] - 2018-12-21

### Fixed

- Add Spotify region EU to device in config. If the region doesn't exist Sonos will throw 800 (unable to play item)

## [3.1.0] - 2018-12-21

### Added

- Add shorthand commands for `getqueue`, `currentqueue` and `fullqueue` #36
- Helpful message when trying to find now playing of empty queue #30

### Changed

- Move Sonos bindings to a separate npm package, [bs-sonos](https://www.npmjs.com/package/bs-sonos) #37

### Fixed

- Invalid dates in the database #31
- Update group volume #32

## [3.0.2] - 2018-12-05

### Fixed

- Queue numbering should be bumped by 1 to not start at 0
- Play and queue when queue is empty #28
- Handle special character `&amp;` #27
- Event in JSON is optional
- Handle :thumbsup: and :thumbsdown:

## [3.0.1] - 2018-11-23

### Fixed

- Fix broken easter egg emoji in help #22

## [3.0.0] - 2018-11-23

### Breaking

- Use device IP from environment variable `WEJAY_IP` #21

### Changed

- Only build using Travis CI #19

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
