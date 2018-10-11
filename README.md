# Wejay Sonos

## Get started

```
$ git clone https://github.com/Iteam1337/sonos-wejay.git
$ cd sonos-wejay
$ npm install
```

In one terminal run `npm start`, this will compile the Reason code to JavaScript.
In another terminal run `npm run dev` to start the Express service.

### POST /event

This is the basic POST data needed for a response. See `decodeCommand` in `Decode.re` for all available commands (first value in `event.text`).

```
{
  "event": {
    "text": "search test",
    "channel": "ABC"
  },
  "type": "event_callback"
}
```
