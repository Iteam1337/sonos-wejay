# Sonos Wejay

[![Build Status](https://travis-ci.com/Iteam1337/sonos-wejay.svg?token=pRHtrm4YwVYpN8sqXadx&branch=master)](https://travis-ci.com/Iteam1337/sonos-wejay)

Wejay is a music collaboration bot for Slack that can be used to add music to a local Sonos system. It needs to be started on the same network as your Sonos system in order to communicate with the speakers.

![Demo of Wejay Slack bot](/docs/wejay.gif)

## Get started

### Prerequisites

Start by creating a `.env` file in the root directory of the project. This is for the Spotify search feature and to allow the bot to interact with Slack. It should contain

```
SPOTIFY_ID=spotify-token-here
SPOTIFY_SECRET=spotify-secret-here
SLACK_TOKEN=slack-token-here
WEJAY_IP=ip-of-the-speaker
```

Spotify token/secret can be generated by creating an app at https://developer.spotify.com/. Slack token can be found after adding the bot to your Slack below. `WEJAY_IP` is found in "About My Sonos System" of the Sonos Controller app.

```
$ git clone https://github.com/Iteam1337/sonos-wejay.git
$ cd sonos-wejay
$ npm install
$ npm build && npm run dev
```

This will compile the Reason code and start up the Express app.

## Testing it out

1. The easiest way to start testing is using [ngrok](https://ngrok.com/). After you've installed `ngrok`, open another terminal and run `ngrok http 3000`. This will create a tunnel to you local environment.
2. Go to https://api.slack.com/apps and create a new app
3. Inside _OAuth & Permissions_ you'll find "Bot User OAuth Access Token" which is the token that should be added in your `.env` above.
4. Go to _Interactive Components_ and enable it. In the request url enter one of the urls provided by `ngrok` (http or https) and the route `/action`, i.e. https://127.0.0.1/action
5. Also on _Interactive Components_ add a action. Give it the name "Queue track" and description and callback id "queue"
6. Go to _Event Subscriptions_ and enable it. In the request url use the same url as above but use the route `/event`
7. Also on _Event Subscriptions_ add a "Bot User Event". The best one is `message.im`, which means you can send commands to the bot in private messages
8. You should be able to send some commands to the bot in Slack now, try sending `help` to see all available commands

## Available commands

| Command                        | Shorthand                | Description                                                  |
| ------------------------------ | ------------------------ | ------------------------------------------------------------ |
| `blame`                        |                          | Who added the track? (requires MySQL database)               |
| `clear`                        |                          | Reset playlist                                               |
| `currentqueue`                 | `getqueue`               | Display upcoming tracks                                      |
| `fullqueue`                    |                          | Display all tracks currently in the queue                    |
| `help`                         |                          | Display the help text                                        |
| `library <your search string>` | `l <your search string>` | Search in tracks added to the Sonos music library            |
| `mute`                         |                          | Mute                                                         |
| `mostplayed`                   |                          | List the most played tracks (requires MySQL database)        |
| `next`                         |                          | Skip one track forward                                       |
| `nowplaying`                   | `np`                     | Display what's playing right now                             |
| `pause`                        |                          | Pause                                                        |
| `play`                         |                          | Play                                                         |
| `playtrack <trackNumber>`      |                          | Play track number                                            |
| `previous`                     |                          | Go back one track                                            |
| `queue <Spotify URI>`          | `q <Spotify URI>`        | Queue a track using a Spotify URI                            |
| `search <your search string>`  | `s <your search string>` | Search Spotify for a track                                   |
| `toplist`                      |                          | List top Wejayers (requires MySQL database)                  |
| `unmute`                       |                          | Unmute                                                       |
| `volume <0-100>`               |                          | Set volume, if no number is provided displays current volume |

### Copy and paste from Spotify

You can also select a couple of tracks in Spotify, press `cmd+c` (Mac) / `ctrl+c` (Windows), paste the copied information to the Wejay bot and send the message. Wejay will add all of your tracks at the end of the queue! :tada:

## Database

Some of the commands, like `blame` and `toplist` requires that you have a MySQL database that the service can access. We've included a `docker-compose.yml` that you can use to spin up a database.

## Contributors

Thanks goes to these wonderful people ([emoji key](https://github.com/kentcdodds/all-contributors#emoji-key)):

<!-- ALL-CONTRIBUTORS-LIST:START - Do not remove or modify this section -->
<!-- prettier-ignore -->
| [<img src="https://avatars1.githubusercontent.com/u/1478102?v=4" width="100px;"/><br /><sub><b>Rickard Laurin</b></sub>](https://willcodefor.beer/)<br />[💻](https://github.com/Iteam1337/Sonos Wejay/commits?author=believer "Code") [🐛](https://github.com/Iteam1337/Sonos Wejay/issues?q=author%3Abeliever "Bug reports") [📖](https://github.com/Iteam1337/Sonos Wejay/commits?author=believer "Documentation") [🤔](#ideas-believer "Ideas, Planning, & Feedback") [👀](#review-believer "Reviewed Pull Requests") [⚠️](https://github.com/Iteam1337/Sonos Wejay/commits?author=believer "Tests") | [<img src="https://avatars3.githubusercontent.com/u/17602389?v=4" width="100px;"/><br /><sub><b>Tom Ekander</b></sub>](https://github.com/lessp)<br />[💻](https://github.com/Iteam1337/Sonos Wejay/commits?author=lessp "Code") [⚠️](https://github.com/Iteam1337/Sonos Wejay/commits?author=lessp "Tests") [🤔](#ideas-lessp "Ideas, Planning, & Feedback") |
| :---: | :---: |
<!-- ALL-CONTRIBUTORS-LIST:END -->

This project follows the [all-contributors](https://github.com/kentcdodds/all-contributors) specification. Contributions of any kind welcome!