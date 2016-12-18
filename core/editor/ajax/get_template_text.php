<?php

clean_magic_quotes_once($_POST['tpl_data']);
$obj=json_decode($_POST['tpl_data']);
echo compile_tpl($obj);

?>