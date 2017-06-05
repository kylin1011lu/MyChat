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
