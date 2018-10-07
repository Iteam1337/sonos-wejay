const axios = require('axios')
const {
  queueTrack,
  playTrack,
  pauseTrack,
  clearPlaylist,
  currentQueue,
  currentTrack,
  setVolume,
  mute,
  previousTrack,
  nextTrack,
} = require('./lib/js/src/Demo.bs')
const { searchTrack } = require('./lib/js/src/Spotify.bs')
const { sendSlackResponse } = require('./lib/js/src/Slack.bs')
const queryString = require('query-string')

const queueEasterEgg = async (channel, track) => {
  await clearPlaylist(channel)

  for (let i = 0; i < 10; i++) {
    await queueTrack(track)
  }

  await playTrack(channel)
}

const event = async (req, res) => {
  const { event, type } = req.body

  if (type === 'url_verification') {
    res.send(req.body.challenge)
    return
  }

  if (type === 'event_callback') {
    const { channel, text } = event

    if (!text) {
      res.sendStatus(200)
      return
    }

    if (text.toLowerCase() === 'friday') {
      const isFriday = new Date().getDay() === 5

      if (isFriday) {
        await queueEasterEgg(channel, 'spotify:track:4fK6E2UywZTJIa5kWnCD6x')
      } else {
        await sendSlackResponse(channel, "Sorry, it's not Friday")
      }

      res.sendStatus(200)
      return
    }

    if (text.toLowerCase() === 'spela shoreline') {
      await queueEasterEgg(channel, 'spotify:track:77jVczOFXfbdugN4djsIqs')

      res.sendStatus(200)
      return
    }

    if (text.toLowerCase() === 'free bird') {
      await queueEasterEgg(channel, 'spotify:track:4qsAYBCJnu2OkTKUVbbOF1')

      res.sendStatus(200)
      return
    }

    if (text.toLowerCase() === 'tequila') {
      await queueEasterEgg(channel, 'spotify:track:5gJKsGij5oGt5H5RSFYXPa')

      res.sendStatus(200)
      return
    }

    switch (text.split(' ')[0].toLowerCase()) {
      case 'queue':
        const track = text
          .split(' ')[1]
          .replace('<', '')
          .replace('>', '')

        await queueTrack(track)
        break

      case 'play':
        await playTrack(channel)
        break

      case 'pause':
        await pauseTrack(channel)
        break

      case 'mute':
        await mute(channel, true)
        break

      case 'unmute':
        await mute(channel, false)
        break

      case 'current':
        const currentlyPlaying = await currentTrack()
        await sendSlackResponse(
          channel,
          `*Currently playing*\n${currentlyPlaying.artist} - ${
            currentlyPlaying.title
          }`
        )
        break

      case 'upcomingtracks':
        const queue = await currentQueue()
        const nowPlaying = await currentTrack()

        const queueAttachments = queue.items
          .slice(nowPlaying.queuePosition)
          .map((track, i) => ({
            text: `${i + 1}. ${track.artist} - ${track.title}`,
          }))

        await sendSlackResponse(channel, 'Upcoming tracks', queueAttachments)
        break

      case 'clear':
        await clearPlaylist(channel)
        break

      case 'previous':
        await previousTrack(channel)
        break

      case 'next':
        await nextTrack(channel)
        break

      case 'volume':
        const volume = text.split(' ')[1]
        await setVolume(channel, volume)
        break

      case 'search':
        const query = text
          .split(' ')
          .slice(1)
          .join('%20')
        const tracks = await searchTrack(query)
        const attachments = tracks.data.tracks.items.map(track => ({
          text: `${track.artists.map(artist => artist.name).join(', ')} - ${
            track.name
          }`,
          callback_id: 'queue',
          actions: [
            {
              name: 'track',
              text: 'Queue',
              type: 'button',
              value: track.uri,
            },
          ],
        }))

        await sendSlackResponse(channel, 'Tracks:', attachments)
        break
    }
  }

  res.sendStatus(200)
}

const spotify = async (req, res) => {
  try {
    const response = await axios.post(
      'https://accounts.spotify.com/api/token',
      queryString.stringify({
        grant_type: 'client_credentials',
      }),
      {
        headers: {
          Authorization:
            'Basic ' +
            new Buffer.from(
              '8f6a97af1d5d481a80db1c06fd648b56:dd9a892639d34b8cad53b00654a89548'
            ).toString('base64'),
        },
      }
    )

    res.send(response.data)
  } catch (error) {
    throw new Error(error)
  }
}

const actions = async (req, res) => {
  const { actions } = JSON.parse(req.body.payload)

  await queueTrack(actions[0].value)

  res.send('Queued!')
}

module.exports = {
  actions,
  event,
  spotify,
}
