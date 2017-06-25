--
-- `acct`
--
DROP TABLE IF EXISTS `acct`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `acct` (
    `id` INT UNSIGNED NOT NULL,
    `name` VARCHAR(20) NOT NULL,
    `pwd` VARCHAR(20) NOT NULL,
    PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='acct table';
SET character_set_client = @saved_cs_client;


--
-- `message`
--
DROP TABLE IF EXISTS `message`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `message` (
    `send_userid` INT UNSIGNED NOT NULL,
    `send_time` TIMESTAMP NOT NULL,
    `chat_content` TEXT NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='message table';
SET character_set_client = @saved_cs_client;

--
-- `last_message`
--
DROP TABLE IF EXISTS `last_message`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `last_message` (
    `userid` INT UNSIGNED NOT NULL,
    `last_time` TIMESTAMP NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='last_message table';
SET character_set_client = @saved_cs_client;
