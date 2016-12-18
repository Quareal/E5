CREATE TABLE `ex_group` (
  `ex_id` int(11) NOT NULL AUTO_INCREMENT,
  `ex_module` int(11) DEFAULT NULL,
  `ex_table` int(11) DEFAULT NULL,
  `ex_ex1` int(11) DEFAULT NULL,
  `ex_ex2` int(11) DEFAULT NULL,
  PRIMARY KEY (`ex_id`),
  KEY `ex_ex1` (`ex_ex1`),
  KEY `ex_ex2` (`ex_ex2`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;

CREATE TABLE `ex_module` (
  `ex_id` int(11) NOT NULL AUTO_INCREMENT,
  `ex_module` int(11) DEFAULT NULL,
  `ex_name` text,
  `ex_sname` text,
  `ex_uin` text,
  `ex_major` int(11) DEFAULT NULL,
  `ex_public` tinyint(4) DEFAULT NULL,
  PRIMARY KEY (`ex_id`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;

CREATE TABLE `ex_table` (
  `ex_id` int(11) NOT NULL AUTO_INCREMENT,
  `ex_table` int(11) DEFAULT NULL,
  `ex_module` int(11) DEFAULT NULL,
  `ex_name` text,
  PRIMARY KEY (`ex_id`),
  KEY `ex_table` (`ex_table`),
  KEY `ex_module` (`ex_module`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;

CREATE TABLE `ex_zone` (
  `ex_id` int(11) NOT NULL AUTO_INCREMENT,
  `ex_zone` int(11) DEFAULT NULL,
  `ex_module` int(11) DEFAULT NULL,
  `ex_module2` int(11) DEFAULT NULL,
  PRIMARY KEY (`ex_id`),
  KEY `ex_zone` (`ex_zone`),
  KEY `ex_module` (`ex_module`),
  KEY `ex_module2` (`ex_module2`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;

CREATE TABLE `main_col` (
  `col_id` int(11) NOT NULL AUTO_INCREMENT,
  `col_module` int(11) DEFAULT NULL,
  `col_table` int(11) DEFAULT NULL,
  `col_cat` int(11) DEFAULT NULL,
  `col_name` text,
  `col_sname` text,
  `col_uin` text,
  `col_type` tinyint(4) DEFAULT NULL,
  `col_target` tinyint(4) DEFAULT NULL,
  `col_link` int(11) DEFAULT NULL,
  `col_link2` tinyint(4) DEFAULT NULL,
  `col_link3` tinyint(4) DEFAULT NULL,
  `col_link4` tinyint(4) DEFAULT NULL,
  `col_index` tinyint(4) DEFAULT NULL,
  `col_deep` TINYTEXT DEFAULT NULL,
  `col_pos` int(11) DEFAULT NULL,
  `col_order` int(11) DEFAULT NULL,
  `col_bold` tinyint(4) DEFAULT NULL,
  `col_filter` int(11) DEFAULT NULL,
  `col_paramlink` int(11) DEFAULT NULL,
  `col_default` text,
  `col_deflist` text,
  `col_unique` tinyint(4) DEFAULT NULL,
  `col_required` tinyint(4) DEFAULT NULL,
  `col_url` tinyint(4) DEFAULT NULL,
  `col_tpl` tinyint(4) DEFAULT NULL,
  `module_url` tinyint(4) DEFAULT NULL,
  `module_type` int(11) DEFAULT NULL,
  `file_dir` text,
  `file_maxsize` int(11) DEFAULT NULL,
  `file_prefix` text,
  `file_types` text,
  `file_totalmax` int(11) DEFAULT NULL,
  `file_genname` tinyint(4) DEFAULT NULL,
  `col_inform` tinyint(4) DEFAULT NULL,
  `col_fastedit` tinyint(4) DEFAULT NULL,
  `col_part` int(11) DEFAULT NULL,
  `col_speclink` text,
  `col_onshow` text,
  `col_force_onshow` tinyint(4) DEFAULT NULL,
  `col_onform` text,
  `col_hint` text,
  `col_oninsert` text,
  `col_parts` longtext,
  `col_date` datetime DEFAULT '0000-00-00 00:00:00',
  `col_date2` datetime DEFAULT '0000-00-00 00:00:00',
  PRIMARY KEY (`col_id`),
  KEY `col_module` (`col_module`),
  KEY `col_table` (`col_table`),
  KEY `col_bold` (`col_bold`),
  KEY `col_url` (`col_url`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;

CREATE TABLE `main_module` (
  `module_id` int(11) NOT NULL AUTO_INCREMENT,
  `module_name` text,
  `module_sname` text,
  `module_uin` text,
  `module_icon` text,
  `module_data` longtext,
  `module_parts` longtext,
  `module_date` datetime DEFAULT '0000-00-00 00:00:00',
  `module_date2` datetime DEFAULT '0000-00-00 00:00:00',
  `module_lastcheck` date DEFAULT '0000-00-00',
  `module_major` tinyint(4) DEFAULT NULL,
  `module_public_ex` int(11) DEFAULT NULL,
  PRIMARY KEY (`module_id`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;

CREATE TABLE `main_part` (
  `part_id` int(11) NOT NULL AUTO_INCREMENT,
  `part_module` int(11) DEFAULT NULL,
  `part_name` text,
  `part_sname` text,
  `part_uin` text,
  `part_date` datetime DEFAULT '0000-00-00 00:00:00',
  `part_date2` datetime DEFAULT '0000-00-00 00:00:00',
  `part_auth` INT DEFAULT '-1',
  `part_about` text,
  `part_access` tinyint(4) DEFAULT NULL,
  `part_type` tinyint(4) DEFAULT NULL,
  `part_table` int(11) DEFAULT NULL,
  `part_owner` int(11) DEFAULT NULL, 
  `part_body` longtext,
  `part_url` text,
  `part_ignore` tinyint(4) DEFAULT NULL,
  `part_iowner` tinyint(4) DEFAULT NULL,
  `part_sowner` tinyint(4) DEFAULT NULL,
  `part_parse` tinyint(4) DEFAULT NULL,
  `part_cat` int(11) DEFAULT NULL,
  `part_major` int(11) DEFAULT NULL,
  `part_proc` tinyint(4) DEFAULT NULL,
  `part_ex` int(11) DEFAULT NULL,
  `part_parts` longtext,
  `part_404` tinyint(4) DEFAULT NULL ,
  `timer_last` datetime DEFAULT '0000-00-00 00:00:00',
  `timer_act` tinyint(4) DEFAULT NULL,
  `timer_type` tinyint(4) DEFAULT NULL,
  `timer_date` date DEFAULT '0000-00-00',
  `timer_time` time DEFAULT '00:00:00',
  `timer_x` int(11) DEFAULT NULL,
  `timer_y` int(11) DEFAULT NULL,
  `timer_count` int(11) DEFAULT NULL,
  `part_file` text,
  `parser_end_ex` int(11) DEFAULT NULL,
  `part_shell` datetime DEFAULT '0000-00-00 00:00:00',
  `part_cur` text,
  `part_pic` text,
  `part_folder` text,
  `part_ifcase` text,
  `part_ifdetect` text,
  `part_ifrow` text,
  `part_unsafe` tinyint(4) DEFAULT NULL,
  `part_shelltime` float(11) DEFAULT NULL,
  `part_shellcount` int(11) DEFAULT NULL,
  `part_skipurl` int(11) DEFAULT NULL,
  `part_enable` tinyint(4) DEFAULT '1',
  PRIMARY KEY (`part_id`),
  KEY `part_module` (`part_module`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;

CREATE TABLE `main_row` (
  `row_id` int(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  `row_module` int(11) DEFAULT NULL,
  `row_table` int(11) DEFAULT NULL,
  `row_ex` int(11) DEFAULT NULL,
  `row_enable` tinyint(4) DEFAULT '1',
  `row_sub` int(11) UNSIGNED DEFAULT NULL,
  `row_user` int(11) DEFAULT NULL, 
  `row_uin` TINYTEXT NOT NULL,
  `backup_row` int(11) UNSIGNED DEFAULT NULL,
  `backup_date` datetime DEFAULT '0000-00-00 00:00:00',
  `modified_date` datetime DEFAULT '0000-00-00 00:00:00',
  `creation_date` datetime DEFAULT  '0000-00-00 00:00:00',
  PRIMARY KEY (`row_id`),
  KEY `row_module` (`row_module`),
  KEY `row_table` (`row_table`),
  KEY `row_ex` (`row_ex`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;

CREATE TABLE `main_table` (
  `table_id` int(11) NOT NULL AUTO_INCREMENT,
  `table_name` text,
  `table_sname` text,
  `table_uin` text,
  `table_icon` text,
  `table_module` int(11) DEFAULT NULL,
  `table_multy` tinyint(4) DEFAULT NULL,
  `table_cansub` tinyint(4) DEFAULT NULL,
  `major_col` int(11) DEFAULT NULL,
  `table_bold` tinyint(4) DEFAULT NULL,
  `table_onedit` text,
  `table_extype` tinyint(4) DEFAULT NULL,
  `table_counter` text,
  `table_bottom` text,
  `table_top` text,
  `table_parts` longtext,
  `table_date` datetime DEFAULT '0000-00-00 00:00:00',
  `table_date2` datetime DEFAULT '0000-00-00 00:00:00',
  `table_public` tinyint(4) DEFAULT NULL,
  PRIMARY KEY (`table_id`),
  KEY `table_module` (`table_module`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;

CREATE TABLE `main_zone` (
  `zone_id` int(11) NOT NULL AUTO_INCREMENT,
  `zone_active` int(11) DEFAULT '1',
  `zone_domain` text,
  `zone_email` text,
  `zone_folder` text,
  `zone_iprange` text,
  `zone_robots` text,
  `zone_safe` TINYINT DEFAULT '0',
  `zone_tpl` text,
  `zone_autosub` tinyint(4) DEFAULT NULL,
  `zone_redirect` int(11) DEFAULT NULL,
  `zone_module` tinytext,
  `zone_name` text,
  PRIMARY KEY (`zone_id`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;

CREATE TABLE `part_cat` (
  `cat_id` int(11) NOT NULL AUTO_INCREMENT,
  `cat_name` text,
  `cat_uin` text,
  `cat_type` tinyint(4) DEFAULT NULL,
  `cat_owner` int(11) DEFAULT NULL,
  `cat_pre` text,
  `cat_after` text,
  PRIMARY KEY (`cat_id`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;

CREATE TABLE `part_param` (
  `param_id` int(11) NOT NULL AUTO_INCREMENT,
  `param_part` int(11) DEFAULT NULL,
  `param_name` text,
  `param_sname` text,
  `param_uin` text,
  `param_default` text,
  `param_array` tinyint(4) DEFAULT NULL,
  `param_type` tinyint(4) DEFAULT NULL,
  `param_list` text,
  `param_link` int(11) DEFAULT NULL,
  `param_get` tinyint(4) DEFAULT NULL,
  `param_hide` tinyint(4) DEFAULT NULL,
  `param_connect` int(11) DEFAULT NULL,
  PRIMARY KEY (`param_id`),
  KEY `param_part` (`param_part`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;

CREATE TABLE `main_mail` (
  `mail_id` int(11) UNSIGNED NOT NULL auto_increment,
  `mail_from` int(11) NOT NULL,
  `mail_to` int(11) NOT NULL,
  `mail_read` text,
  `mail_topic` text,
  `mail_body` text NOT NULL,
  `mail_date` datetime NOT NULL,
  PRIMARY KEY  (`mail_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;

CREATE TABLE `row_owner` (
  `ro_id` int(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  `ro_pos` int(11) DEFAULT NULL,
  `ro_ex` int(11) DEFAULT NULL,
  `ro_sub` int(11) UNSIGNED DEFAULT NULL,
  `ro_enable` tinyint(4) DEFAULT '1',
  `row_id` int(11) UNSIGNED DEFAULT NULL,
  `row_module` int(11) DEFAULT NULL,
  `row_table` int(11) DEFAULT NULL,
  `owner_id` int(11) UNSIGNED DEFAULT NULL,
  `owner_table` int(11) DEFAULT NULL,
  `owner_module` int(11) DEFAULT NULL,
  `ro_user` int(11) DEFAULT NULL, 
  `ro_users` tinyint(4) DEFAULT NULL,  
  PRIMARY KEY (`ro_id`),
  KEY `ro_ex` (`ro_ex`),
  KEY `row_id` (`row_id`),
  KEY `owner_id` (`owner_id`),
  KEY `row_module` (`row_module`),
  KEY `row_table` (`row_table`),
  KEY `owner_module` (`owner_module`),
  KEY `owner_table` (`owner_table`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;

CREATE TABLE `row_user` (
  `ru_id` int(11) NOT NULL AUTO_INCREMENT,
  `ru_user` int(11) DEFAULT NULL,
  `ru_row` int(11) UNSIGNED DEFAULT NULL,
  PRIMARY KEY (`ru_id`),
  KEY `ru_user` (`ru_user`),
  KEY `ru_row` (`ru_row`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;

CREATE TABLE `row_value` (
  `value_id` int(11) UNSIGNED NOT NULL AUTO_INCREMENT,
  `value_module` int(11) DEFAULT NULL,
  `value_table` int(11) DEFAULT NULL,
  `value_row` int(11) UNSIGNED DEFAULT NULL,
  `value_col` int(11) DEFAULT NULL,
  `value_value` longtext,
  PRIMARY KEY (`value_id`),
  KEY `value_table` (`value_table`),
  KEY `value_module` (`value_module`),
  KEY `col_row` (`value_col`, `value_row`),
  KEY `row_table_col` (`value_row`, `value_table` ,`value_col`),
  KEY `value_value` ( `value_value` ( 11 ) )
  
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;

CREATE TABLE `table_sub` (
  `sub_id` int(11) NOT NULL AUTO_INCREMENT,
  `sub_table1` int(11) DEFAULT NULL,
  `sub_table2` int(11) DEFAULT NULL,
  PRIMARY KEY (`sub_id`),
  KEY `sub_table1` (`sub_table1`),
  KEY `sub_table2` (`sub_table2`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;

CREATE TABLE `auth_link` (
  `link_id` int(11) NOT NULL auto_increment,
  `link_user` int(11) NOT NULL,
  `link_group` int(11) NOT NULL,
  `link_expire` date NOT NULL,
  `link_date` date NOT NULL,
  `link_invite` int(11) NOT NULL,
  PRIMARY KEY  (`link_id`),
  KEY `link_user` (`link_user`),
  KEY `link_group` (`link_group`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;

CREATE TABLE `auth_perm` (
  `perm_id` int(11) UNSIGNED NOT NULL auto_increment,
  `perm_target` tinyint(4) NOT NULL,
  `perm_type` tinyint(4) NOT NULL,
  `perm_auth` int(11) NOT NULL,
  `perm_object` int(11) NOT NULL,
  `perm_folder` text NOT NULL,
  `perm_subtable` text NOT NULL,
  `perm_view` tinyint(4) default '-1',
  `perm_edit` tinyint(4) default '-1',
  `perm_add` tinyint(4) default '-1',
  `perm_del` tinyint(4) default '-1',
  `perm_control` tinyint(4) default '-1',
  `perm_rules` tinyint(4) default '-1',
  `perm_upload` tinyint(4) default '-1',
  `perm_maxupl` int(11) NOT NULL,
  `perm_reg` tinyint(4) default '-1',
  `perm_invite` tinyint(4) default '-1',
  `perm_leave` tinyint(4) default '-1',
  `perm_unreg` tinyint(4) default '-1',
  PRIMARY KEY  (`perm_id`),
  KEY `perm_auth` (`perm_auth`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;

CREATE TABLE `main_auth` (
  `auth_id` int(11) NOT NULL auto_increment,
  `auth_type` tinyint(4) NOT NULL,
  `auth_owner` text NOT NULL,
  `auth_date` date NOT NULL,
  `user_login` text NOT NULL,
  `user_pwl` text NOT NULL,
  `user_fixedip` text NOT NULL,
  `user_email` text NOT NULL,
  `user_pwlcode` tinyint(4) NOT NULL,
  `user_name` text NOT NULL,
  `user_safe` text NOT NULL,
  `user_lastlogin` int(11) NOT NULL,
  `user_row` int(11) UNSIGNED NOT NULL,
  `group_name` text NOT NULL,
  `group_sname` text NOT NULL,
  `group_module` int(11) NOT NULL,
  `group_table` int(11) NOT NULL,
  `group_owner` int(11) NOT NULL,
  `group_uin` text NOT NULL,
  `user_ip` TINYTEXT NOT NULL,
  `auth_enable` tinyint(4) NOT NULL default '1',
  `session_lifetime` int(11) NOT NULL,
  `session_multy` int(11) NOT NULL,
  PRIMARY KEY  (`auth_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;

CREATE TABLE `auth_session` (
  `session_id` int(11) NOT NULL auto_increment,
  `session_auth` int(11) NOT NULL,
  `session_start` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `session_last` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `session_over` int(11) NOT NULL,
  `session_active` tinyint(4) NOT NULL,
  `session_hash` tinytext NOT NULL,
  `session_ip` tinytext NOT NULL,
  PRIMARY KEY  (`session_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;

CREATE TABLE `main_news` (
  `news_id` INT UNSIGNED NOT NULL AUTO_INCREMENT,
  `news_title` TEXT NOT NULL,
  `news_url` TEXT NOT NULL,
  `news_group` INT NOT NULL,
  `news_from` INT NOT NULL,
  `news_fromip` TEXT NOT NULL,
  `news_fromurl` TEXT NOT NULL,
  `news_read` INT( 0 ) NOT NULL,
  `news_module` INT NOT NULL,
  `news_part` INT NOT NULL,
  `news_row` INT UNSIGNED NOT NULL,
  `news_ex` INT NOT NULL,
  `news_datetime` DATETIME NOT NULL,
  `news_day` INT NOT NULL,
PRIMARY KEY (`news_id`),
KEY (`news_group`),
KEY (`news_from`),
KEY (`news_read`),
KEY (`news_module`),
KEY (`news_day`) 
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;

CREATE TABLE `row_index` (
  `index_word` INT UNSIGNED NOT NULL,
  `index_pos` INT NOT NULL,
  `index_row` INT UNSIGNED NOT NULL,
  `index_col` INT NOT NULL,
  `index_tex` INT NOT NULL,
  `index_owner` INT UNSIGNED NOT NULL,
KEY (`index_word`),
KEY (`index_row`),
KEY (`index_col`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;

CREATE TABLE `index_word` (
  `word_id` INT UNSIGNED NOT NULL AUTO_INCREMENT,
  `word_lemma` VARCHAR(20) NOT NULL,
PRIMARY KEY (`word_id`),
KEY ( `word_lemma` ) 
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;

CREATE TABLE `visit_ip` (
  `ip_zone` INT NOT NULL,
  `ip_type` TINYINT NOT NULL,
  `ip_action` INT NOT NULL,
  `ip_value` INT UNSIGNED NOT NULL,
  `ip_visit` INT NOT NULL,
  `ip_time` INT NOT NULL,
KEY (`ip_type`),
KEY (`ip_zone`),
KEY (`ip_value`),
KEY (`ip_time`) 
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;

CREATE TABLE `visit_object` (
  `visit_type` TINYINT NOT NULL,
  `visit_object` INT NOT NULL,
  `visit_period` TINYINT NOT NULL,
  `visit_time` INT NOT NULL,
  `visit_host` INT UNSIGNED NOT NULL,
  `visit_hit` INT UNSIGNED NOT NULL,
  `visit_zone` INT NOT NULL,
KEY (`visit_type`),
KEY (`visit_object`),
KEY (`visit_period`),
KEY (`visit_time`),
KEY (`visit_zone`),
KEY `period_time` (`visit_period`,`visit_time`),
KEY `type_period_time` (`visit_type`,`visit_period`,`visit_time`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;

CREATE TABLE `visit_data` (
  `data_id` INT UNSIGNED NOT NULL AUTO_INCREMENT ,
  `data_name` VARCHAR( 200 ) NOT NULL ,
  `data_type` TINYINT NOT NULL ,
PRIMARY KEY ( `data_id` ) ,
KEY ( `data_name`),
KEY (`data_type` ) 
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;

CREATE TABLE `visit_source` (
  `source_zone` INT NOT NULL ,
  `source_type` TINYINT NOT NULL ,
  `source_domain` INT UNSIGNED NOT NULL ,
  `source_data` INT UNSIGNED NOT NULL ,
  `source_day` INT NOT NULL ,
  `source_visit` INT NOT NULL ,
KEY (`source_zone`),
KEY (`source_type`),
KEY (`source_domain`),
KEY (`source_day`) 
);

