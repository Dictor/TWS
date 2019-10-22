<?php
    namespace Dictor\OSAM2019;
    
    /*
        TABLE log(time TEXT, data TEXT)
        TABLE event(time TEXT, kind TEXT, data INTEGER)
        TABLE sensor(time TEXT, temp INTEGER, humi INTEGER, water INTEGER, sound INTEGER)
    */
    class DB {
        private static $currentDB;
		private static $isOpen = FALSE;

        public static function Open($path) {
			try{
				if(!is_dir(dirname($path))) mkdir(dirname($path), 0777, TRUE);
				if(!is_file($path)){
					$db = new \SQLite3($path);
                    $db->exec('CREATE TABLE log(time TEXT NOT NULL, data TEXT);');
                    $db->exec('CREATE TABLE event(time TEXT NOT NULL, kind TEXT, data INTEGER);');
                    $db->exec('CREATE TABLE sensor(time TEXT NOT NULL, temp INTEGER, humi INTEGER, water INTEGER, sound INTEGER);');
					$db->close();
				}
				
				if(!self::$isOpen){
					$db = new \SQLite3($path);
					self::$currentDB = $db;
					if ($db->lastErrorCode() == 0) self::$isOpen = TRUE;
					return $db->lastErrorCode();	
				} else {
					return 0;
				}
			} catch(\Throwable $t) {
				return -1;
			}
        }
        
        public static function Close() {
			self::$currentDB->close();
		}
		
		public static function Execute(string $preQuery, array $parameter) {
			return self::Query($preQuery, $parameter);
		}
		
		public static function Query(string $preQuery, array $parameter) {
			$state = self::$currentDB->prepare($preQuery);
			if($state == FALSE) return FALSE;
			foreach($parameter as $nowkey => $nowval) {
				$state->bindValue($nowkey, $nowval);
			}
			return $state->execute();
		}
    }

    class model {
        public static function UseDB() {
            return DB::Open(config::DB_PATH);
        }
        
        public static function GetLogs(int $daysbefore) {

        }

        public static function GetEvents(int $daysbefore) {

        }

        public static function GetSensorData(int $daysbefore) {
            
        }
    }
?>