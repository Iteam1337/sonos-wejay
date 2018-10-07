const queueEasterEgg = async (channel, track) => {
  await clearPlaylist(channel)

  for (let i = 0; i < 10; i++) {
    await queueTrack(track)
  }

  await playTrack(channel)
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
