<?php


$cmd['trigger'][CMD_STAT]->process=function(&$data,&$op,&$ops){
		$v=parse_var($op->put);
		$GLOBALS["statistics_action"]=$v;
};
$cmd['trigger'][CMD_STAT]->result=CMD_NONE;
$cmd['trigger'][CMD_STAT]->visual=Array('title'=>'триггер','put'=>Array(
		0=>Array('title'=>'Сообщение','type'=>CMD_STRING,'req'=>1)
));


$cmd['stop'][CMD_STAT]->process=function(&$data,&$op,&$ops){
		$GLOBALS["stop_stat"]=true;
};
$cmd['stop'][CMD_STAT]->result=CMD_NONE;
$cmd['stop'][CMD_STAT]->visual=Array('title'=>'остановить');


$cmd['online'][CMD_STAT]->process=function(&$data,&$op,&$ops){
		return get_online($GLOBALS["zi"]);
};
$cmd['online'][CMD_STAT]->result=CMD_STRING;
$cmd['online'][CMD_STAT]->visual=Array('title'=>'количество посетителей online');


$cmd['check'][CMD_STAT]->process=function(&$data,&$op,&$ops){
		check_single($op->put,$GLOBALS["lex_func_sep"]);
		$action=parse_var($op->put[0]);
		if(!empty($op->put[1])) $count=parse_var($op->put[1]); else $count=1;
		if(!empty($op->put[2])) $hours=parse_var($op->put[2]); else $hours=0;
		return check_visit_ip(1,0,'',$action,$count,$hours); //ZONE и IP подставляются автоматически в самой функции
};
$cmd['check'][CMD_STAT]->result=CMD_NONE;
$cmd['check'][CMD_STAT]->visual=Array('title'=>'отметка действия с проверкой','put'=>Array(
		0=>Array('title'=>'название действия','type'=>CMD_STRING,'req'=>1),
		1=>Array('title'=>'макс.кол-во действий','type'=>CMD_STRING,'req'=>1),
		2=>Array('title'=>'период отслеживания (в часах, "!в минутах"), 0 - за весь период сбора статистики','type'=>CMD_STRING,'req'=>0)
));


$cmd['view_action'][CMD_STAT]->process=function(&$data,&$op,&$ops){
		check_single($op->put,$GLOBALS["lex_func_sep"]);
		$action=parse_var($op->put[0]);
		if(!empty($op->put[1])) $hours=parse_var($op->put[1]); else $hours=0;
		return check_visit_ip(1,0,'',$action,0,$hours,1);
};
$cmd['view_action'][CMD_STAT]->result=CMD_NONE;
$cmd['view_action'][CMD_STAT]->visual=Array('title'=>'просмотр количества действия (без отметки)','put'=>Array(
		0=>Array('title'=>'название действия','type'=>CMD_STRING,'req'=>1),
		1=>Array('title'=>'период отслеживания (в часах, "!в минутах"), 0 - за весь период сбора статистики','type'=>CMD_STRING,'req'=>0)
));

?>