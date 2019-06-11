module Spotify = {
  module Track = {
    type t = {
      albumName: string,
      artist: string,
      cover: string,
      duration: int,
      id: string,
      name: string,
      releaseDate: string,
      uri: string,
    };

    let of_json = json =>
      Ezjsonm.{
        albumName: get_string(find(json, ["albumName"])),
        artist: get_string(find(json, ["artist"])),
        cover: get_string(find(json, ["cover"])),
        duration: get_int(find(json, ["duration"])),
        id: get_string(find(json, ["id"])),
        name: get_string(find(json, ["name"])),
        releaseDate: get_string(find(json, ["releaseDate"])),
        uri: get_string(find(json, ["uri"])),
      };

    let to_json = track =>
      Ezjsonm.(
        dict([
          ("albumName", string(track.albumName)),
          ("artist", string(track.artist)),
          ("cover", string(track.cover)),
          ("duration", int(track.duration)),
          ("id", string(track.id)),
          ("name", string(track.name)),
          ("releaseDate", string(track.releaseDate)),
          ("uri", string(track.uri)),
        ])
      );
  };

  module Tracks = {
    type t = list(Track.t);

    let of_json = json =>
      Ezjsonm.(find(json, ["tracks"]) |> get_list(Track.of_json));

    let to_json = json => Ezjsonm.(json |> list(Track.to_json));
  };
};
