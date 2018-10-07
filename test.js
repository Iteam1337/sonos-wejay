const express = require('express')
const bodyparser = require('body-parser')
const app = express()
const routes = require('./routes')

app.use(bodyparser.json())
app.use(bodyparser.urlencoded({ extended: false }))

app.get('/spotify', routes.spotify)

app.post('/event', routes.event)
app.post('/actions', routes.actions)

app.listen(4000, () => console.log(`Example app listening on port 4000!`))
