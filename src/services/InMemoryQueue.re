open Belt.List;

type user = {
  id: string,
  lastPlay: float,
};

type trackWithUser = {
  id: string,
  lastPlay: float,
  track: string,
};

type users = list((string, user));
type t = list((string, list(string)));

type action =
  | AddTrack((option(string), string))
  | ClearQueue
  | RemoveTrack((option(string), string));

module Time = {
  let now = () => Js.Date.make() |> Js.Date.getTime;
};

let currentState: ref(t) = ref([]);
let usersState: ref(users) = ref([]);

let getQueue = () => {
  let state = currentState^;
  let users = usersState^;

  users
  ->sort(((_, a), (_, b)) => (a.lastPlay -. b.lastPlay)->int_of_float)
  ->map(((_, user)) =>
      switch (state->getAssoc(user.id, (===))) {
      | Some(row) =>
        row
        ->map(track => {id: user.id, lastPlay: user.lastPlay, track})
        ->toArray
      | None => [||]
      }
    )
  ->toArray
  ->Weave.make;
};

let getTrack = index => getQueue()->Belt.Array.get(index);
let currentTrack = () => getTrack(0);
let nextTrack = () => getTrack(1);

let make = action => {
  let state = currentState^;
  let users = usersState^;

  let (newTracks, newUsers) =
    switch (action) {
    | AddTrack((user, uri)) =>
      switch (user) {
      | None => (state, users)
      | Some(user) =>
        let newUsers =
          switch (getAssoc(users, user, (===))) {
          | Some(user) =>
            users->setAssoc(user.id, {...user, lastPlay: Time.now()}, (===))
          | None =>
            users->concat([(user, {id: user, lastPlay: Time.now()})])
          };

        let newTracks =
          switch (getAssoc(state, user, (===))) {
          | Some(row) => state->setAssoc(user, concat(row, [uri]), (===))
          | None => state->concat([(user, [uri])])
          };

        (newTracks, newUsers);
      }

    | RemoveTrack((user, uri)) =>
      switch (user) {
      | None => (state, users)
      | Some(user) =>
        switch (getAssoc(state, user, (===))) {
        | Some(row) =>
          let newTracks =
            state->setAssoc(user, row->keep(track => track != uri), (===));

          (newTracks, users);
        | None => (state, users)
        }
      }

    | ClearQueue => ([], users)
    };

  currentState := newTracks;
  usersState := newUsers;

  ();
};
