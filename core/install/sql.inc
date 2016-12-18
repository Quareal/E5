<?php

if((!empty($_GET['install']) || !empty($_POST['install'])) && file_exists(DOCUMENT_ROOT.'/core/install/package.sql')){
	$GLOBALS['dump_from_file']=1;
	$dump=DOCUMENT_ROOT.'/core/install/package.sql';
} else {

$dump2=file_get_contents(DOCUMENT_ROOT.'/core/install/dump.sql');

$dump=$dump2."

INSERT INTO `main_zone` (`zone_id`, `zone_active`, `zone_domain`, `zone_folder`, `zone_iprange`, `zone_redirect`, `zone_module`, `zone_name`) VALUES
(1, 1, '', '*admin', '', 0, '-1', 'Control');

INSERT INTO `auth_perm` (`perm_id`, `perm_target`, `perm_type`, `perm_auth`, `perm_object`, `perm_folder`, `perm_view`, `perm_edit`, `perm_add`, `perm_del`, `perm_control`, `perm_rules`, `perm_upload`, `perm_maxupl`, `perm_reg`, `perm_invite`, `perm_leave`, `perm_unreg`) VALUES
(1, 1, 0, 0, 1, '', 0, -1, -1, -1, -1, -1, -1, 0, -1, -1, -1, -1);

INSERT INTO `auth_perm` (`perm_id`, `perm_target`, `perm_type`, `perm_auth`, `perm_object`, `perm_folder`, `perm_view`, `perm_edit`, `perm_add`, `perm_del`, `perm_control`, `perm_rules`, `perm_upload`, `perm_maxupl`, `perm_reg`, `perm_invite`, `perm_leave`, `perm_unreg`) VALUES
(2, 0, 4, 0, 0, '', 1, -1, -1, -1, -1, -1, -1, 0, -1, -1, -1, -1)

";

}

?>