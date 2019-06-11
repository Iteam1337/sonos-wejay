# Wejay

## Getting started

```
npm install -g esy
esy
esy build
esy server
```

Server should be running at `http://localhost:3000`.

Make a POST-request with a JSON-body to `/command`, e.g.

```json
{
  "command": "search",
  "args": "hello"
}
```
