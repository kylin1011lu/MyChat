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
