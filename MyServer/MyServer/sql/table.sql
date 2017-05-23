--
-- `acct`
--
DROP TABLE IF EXISTS `acct`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `acct` (
    `id` bigint unsigned NOT NULL,
    `data` longblob NOT NULL,
    PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='acct table';
SET character_set_client = @saved_cs_client;


DROP TABLE IF EXISTS `ios_user`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `ios_user` (
  `id` INT UNSIGNED NOT NULL AUTO_INCREMENT,
  `platform_str_id` VARCHAR(45) NULL,
  `data` TINYBLOB NULL,
  PRIMARY KEY (`id`),
  INDEX `platform_str_id` (`platform_str_id` ASC)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
SET character_set_client = @saved_cs_client;


DROP TABLE IF EXISTS `huawei_user`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `huawei_user` (
  `id` INT UNSIGNED NOT NULL AUTO_INCREMENT,
  `platform_str_id` VARCHAR(45) NULL,
  `data` TINYBLOB NULL,
  PRIMARY KEY (`id`),
  INDEX `platform_str_id` (`platform_str_id` ASC)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
SET character_set_client = @saved_cs_client;


--
-- `user`
--
DROP TABLE IF EXISTS `user`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `user` (
    `id` bigint unsigned NOT NULL,
    `data` blob NOT NULL,
    `var` Blob,
    `attack_log` blob,
    `attack_video` blob,
    `defense_log` blob,
    `defense_video` blob,
    `offline_event` blob,
    `mail` Blob,
    `pve` Blob,
	`friends` Blob,
	`village_model` Blob,
    `temp_data` Blob,
    PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='user table';
SET character_set_client = @saved_cs_client;

--
-- `clan`
--
DROP TABLE IF EXISTS `clan`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `clan` (
    `id` int NOT NULL AUTO_INCREMENT,
    `data` blob NOT NULL,
	`tag1` varchar(64) NOT NULL, 
	`tag2` varchar(44) NOT NULL, 
	`tag3` varchar(24) NOT NULL, 
	`position` varchar(100) NOT NULL, 
	`member_cnt_from` int NOT NULL, 
	`member_cnt_to` int NOT NULL, 
	`join_min_point` int NOT NULL, 
	`join_type` int NOT NULL, 
    PRIMARY KEY (`id`),
	INDEX `tag` (`tag1` ASC, `tag2` ASC, `tag3` ASC, `position` ASC, `member_cnt_from` ASC, `member_cnt_to` ASC, `join_min_point` ASC, `join_type` ASC)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='clan table';
SET character_set_client = @saved_cs_client;

--
-- `order`
--
DROP TABLE IF EXISTS `order`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `order` (
	`id` int unsigned NOT NULL AUTO_INCREMENT,
	`uid` int unsigned NOT NULL,
	`buyid` int unsigned NOT NULL,
	`platform` int unsigned NOT NULL,
	`platform_name` varchar(32) NOT NULL, 
	`price` int unsigned NOT NULL,
	`money` int unsigned NOT NULL,
	`gem` int unsigned NOT NULL,
	`orderid` varchar(255) NOT NULL,
	`app_orderid` varchar(255) NOT NULL,
	`status` int unsigned NOT NULL,
	`receipt` text NOT NULL,
	`time` int unsigned NOT NULL,
	PRIMARY KEY (`id`),
	INDEX (`uid`),
	INDEX (`buyid`),
	INDEX (`platform`),
	INDEX(`orderid`),
	INDEX(`app_orderid`),
	INDEX (`status`),
	INDEX (`time`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='order table';
SET character_set_client = @saved_cs_client;
