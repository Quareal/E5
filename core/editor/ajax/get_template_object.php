<?php

clean_magic_quotes_once($_POST['tpl_data']);
echo json_encode(start_parse_tpl($_POST['tpl_data'],1,1));

?>