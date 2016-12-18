<?php

/* окно вывода содержимого адимнистративного кабинета */

global $cmd;

$cmd['main'][CMD_COMPONENT]->process=function(&$data,&$op,&$ops){
	global $main_content;
	return $main_content;
};
$cmd['main'][CMD_COMPONENT]->result=CMD_STRING;

?>