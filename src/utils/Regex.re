module Patterns = {
  let spotifyTrackURL = [%re "/https:\\/\\/open.spotify.com\\/track\\//ig"];
  let spotifyPlaylistURL = [%re
    "/https:\\/\\/open.spotify.com\\/playlist\\/(\\w+)/"
  ];
  let spotifyPlaylistURLWithoutUser = [%re
    "/https:\\/\\/open.spotify.com\\/playlist\\//"
  ];

  let queryParams = [%re "/\\?si=([^&]*)/"];

  let removeSlackCommandBrackets = [%re "/(<|>)/g"];
  let removeSlackUser = [%re "/<@\\w+>\\s/g"];

  let isEmoji = [%re "/^:[\\w+-:]+:$/"];
  let skinTone = [%re "/:skin-tone-\\d+:/"];
};

let replaceByRe = (str, regex, replaceBy) =>
  Js.String.replaceByRe(regex, replaceBy, str);
