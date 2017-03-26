## Window messages
* open_window x y : ok | err
* mouse_pos: x,y
* button_pressed button_num : true | false
* key_pressed key_num: true | false

## Drawing messages
* draw_sprite sprite_id : ok | err

erlang client impl strategy:

 {ok, S} = gen_tcp:connect({local, <<"/tmp/graphsock.uds">>}, 0,[local,  {packet, 0}, {active, false}]). {ok,#Port<0.511>}
159> gen_tcp:send(S, <<"draw: sp">>).                                                                        ok    
160> {ok, Data} = gen_tcp:recv(S, 20).                                                                      
{ok,[104,101,108,108,111,59,100,114,97,119,59,111,107,59,10,
     0,0,0,0,0]}
161> Words = string:tokens(Data, ";").
["hello","draw","ok",[10,0,0,0,0,0]]
162> Line = string:tokens(Data, "\n").
["hello;draw;ok;",[0,0,0,0,0]]
163> Wordss = string:tokens(Line, ";").
[["hello;draw;ok;",[0,0,0,0,0]]]
164> Cmd = hd(Wordss).
["hello;draw;ok;",[0,0,0,0,0]]
165> Wordsss = string:tokens(hd(Line), ";").
["hello","draw","ok"]
166> Cmdd = hd(Wordsss).
"hello"

  
