-module(game).
-export([start/0, openWindow/0, watch/2, get_new_player_position/2,
					get_drawing_commands_for_player/1] ).


openWindow() ->
	os:cmd("../c_graphics_server/build/app").

watch(Socket, DataList) ->
	%io:format("waiting for incoming data~n"),
	case gen_tcp:recv(Socket, 0, 5) of
		{ok, Data} -> io:format("data: ~s~n~n", [Data]), 
									%timer:sleep(2000),
									watch(Socket, [Data|DataList]);
		{error, timeout} -> %io:format("no new data incoming~n"),
												watch(Socket, DataList);

		{error, Reason} -> io:format("other error: ~p~n", [Reason]);
		_ -> io:format(">>>>>>>>>>>>>>><<< end of transmission: ~p~n", [DataList])
	end.
	

% Retrieves the latest inputs from keyboard and mouse
% and returns them.
get_player_input(Socket) ->
	% ask the graphics/input server for latest input data
	% return a list of input infos
	[{keyboardPress, w}, {mouseButtonDown, left}].


% Based on the player input information, 
% a new position of the player sprite
% is calculated
% and returned as {X, Y}
get_new_player_position([FirstInput|Rest], {X, Y} = Position) ->
	case FirstInput of 
		{keyboardPress, w} -> NewPos = {X, Y+1},
													get_new_player_position(Rest, NewPos);
		{keyboardPress, a} -> NewPos = {X-1, Y},
													get_new_player_position(Rest, NewPos);
		_ -> get_new_player_position(Rest, Position)
	end;
get_new_player_position([], Position) ->
	Position.

get_drawing_commands_for_player({X, Y}) ->
	["draw_sprite;player;" ++ integer_to_list(X) ++ ";" ++  integer_to_list(Y)].

% Physically sending the draw commands to the
% c graphics server
% Returns done when finished.
send_draw_commands([DrawCmd|Rest], Socket) ->
	gen_tcp:send(Socket, DrawCmd),
	send_draw_commands(Rest, Socket);
send_draw_commands([], _) ->
	done.	
	


loop(Socket, PlayerPos) ->
	Input = get_player_input(Socket),
	PlayerPosNew = get_new_player_position(Input, PlayerPos),
	PlayerDrawCommands = get_drawing_commands_for_player(PlayerPosNew),

	% when all drawing commands are collected, send them to 
	% to our c graphics server
	send_draw_commands(PlayerDrawCommands, Socket),
	% add other draw commands for e.g. enemies, background etc.
	loop(Socket, PlayerPosNew).
	

start() ->
	io:format("Game starting~n"),
	io:format("Opening SDL window~n"),
	spawn(game, openWindow, []),
	io:format("window open called~n"),
	io:format("connecting to tcp socket~n"),
	timer:sleep(110),
	{ok, S} = gen_tcp:connect({local, <<"/tmp/graphsock.uds">>}, 0,[local,{nodelay, true},  {packet, 0}, {active, false}]), 
	Pid = spawn(game, watch, [S, []]),
	%watch(S),
	loop(S, {0, 0}),
	io:format("loop entered~n"),
	Pid.
