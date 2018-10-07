const axios = require('axios')
const express = require('express')
const bodyparser = require('body-parser')
const app = express()
const queryString = require('query-string')

app.use(bodyparser.json())
app.use(bodyparser.urlencoded({ extended: false }))

app.get('/spotify', async (req, res) => {
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
})

app.listen(4000, () => console.log(`Example app listening on port 4000!`))
