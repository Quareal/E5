<?php

/* возвращает информацию, необходимую для дебага */

global $cmd;

$cmd['debug'][CMD_COMPONENT]->process=function(&$data,&$op,&$ops){
	$ctmp=start_buffer();
	include(DOCUMENT_ROOT.'/core/units/debug.inc');
	return return_buffer($ctmp);
};
$cmd['debug'][CMD_COMPONENT]->result=CMD_STRING;

?>