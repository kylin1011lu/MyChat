
sql_src=./table.sql
sql_host=localhost
sql_user=root
sql_pass=123456

while getopts "s:h:u:p:dbr" opt
do
	case $opt in
		s ) sql_src=$OPTARG;;
		h ) sql_host=$OPTARG;;
		u ) sql_user=$OPTARG;;
		p ) sql_pass=$OPTARG;;
		d ) sql_database=true;;
		b ) sql_backup=true;;
		r ) sql_restore=true;;
		? ) exit 1;;
	esac
done

if [ $sql_backup ]; then
	if [ ! -d bak ]; then
		mkdir bak
	fi
	for ((i=0; i<8; ++i))
	do
		mysqldump -h $sql_host --user=$sql_user --password=$sql_pass --hex-blob zebra$((i))_n > bak/zebra$((i))_n.sql
	done
	exit 1
fi

if [ $sql_restore ]; then
	for ((i=0; i<8; ++i))
	do
		mysql -h $sql_host --user=$sql_user --password=$sql_pass -D zebra$((i))_n < bak/zebra$((i))_n.sql
	done
	exit 1
fi

if [ $sql_database ]; then
	for ((i=0; i<8; ++i))
	do
		str='DROP DATABASE IF EXISTS `zebra'$i'_n`;'
		str="$str"'CREATE DATABASE `zebra'$i'_n` DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;'		
		echo $str > tmp.sql
		mysql -h $sql_host --user=$sql_user --password=$sql_pass < tmp.sql
	done
	
	# str='CREATE TABLE `zebra0_n`.`game_user` (`id` INT NOT NULL AUTO_INCREMENT, `platform` INT NULL, `platform_num_id` INT NULL,'
	# str=$str'`platform_str_id` VARCHAR(45) NULL, `register_time` INT NULL, PRIMARY KEY (`id`),'
	# str=$str'INDEX `platform_num` (`platform` ASC, `platform_num_id` ASC),'
	# str=$str'INDEX `platform_str` (`platform` ASC, `platform_str_id` ASC));'
	# echo $str > tmp.sql
	# mysql -h $sql_host -u root -D zebra0_n < tmp.sql

	rm tmp.sql -rf
fi

for ((i=0; i<8; i++))
do
	mysql -h $sql_host --user=$sql_user --password=$sql_pass -D zebra$((i))_n < $sql_src
done
