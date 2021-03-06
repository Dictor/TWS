<?php
    namespace Dictor\OSAM2019;
	require_once('config.php');
	
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

		public static function ResultToArray($res) {
			$arr = array();
			while ($nowrow = $res->fetchArray(SQLITE3_ASSOC)){
				if($nowrow != FALSE) $arr[] = $nowrow;
			}
			return $arr;
		}
    }

    class Model {
        public static function UseDB() {
            return DB::Open(config::DB_PATH);
        }
        
        public static function GetLogs(int $daysbefore) {
			if ($daysbefore < 0) {
				return DB::ResultToArray(DB::Query("SELECT * FROM log", array()));
			} else {
				$dt = new \DateTime('now');
				$dt->sub(new \DateInterval('P'.(string)$daysbefore.'D'));
				return DB::Query('SELECT * FROM log WHERE date(time) > date('.$dt->format('Y-m-d').');', array());
			}
        }

        public static function GetEvents(int $daysbefore) {
			if ($daysbefore < 0) {
				return DB::ResultToArray(DB::Query("SELECT * FROM event", array()));
			} else {
				$dt = new \DateTime('now');
				$dt->sub(new \DateInterval('P'.(string)$daysbefore.'D'));
				return DB::Query('SELECT * FROM event WHERE date(time) > date('.$dt->format('Y-m-d').');', array());
			}
        }

        public static function GetSensorData(int $daysbefore) {
            if ($daysbefore < 0) {
				return DB::ResultToArray(DB::Query("SELECT * FROM sensor", array()));
			} else {
				$dt = new \DateTime('now');
				$dt->sub(new \DateInterval('P'.(string)$daysbefore.'D'));
				return DB::Query('SELECT * FROM sensor WHERE date(time) > date('.$dt->format('Y-m-d').');', array());
			}
		}

		public static function GetErrorLog() {
			return DB::ResultToArray(DB::Query('SELECT * FROM log WHERE data LIKE :osmsg', array('osmsg' => '%ERROR,%')));
		}

	
		public static function AddRow(string $date, string $kind, array $data) {
			switch ($kind) {
				case 'LOG': DB::Query("INSERT INTO log(time, data) VALUES (:itime, :idata)", array('itime' => $date, 'idata' => $data[0])); break;
				case 'SENSOR': DB::Query("INSERT INTO sensor(time, temp, humi, water, sound) VALUES (:itime, :itemp, :ihumi, :iwater, :isound)", 
											array('itime' => $date, 'itemp' => (int)$data[0], 'ihumi' => (int)$data[1], 'iwater' => (int)$data[2], 'isound' => (int)$data[3])); break;
				case 'EVENT': DB::Query("INSERT INTO event(time, kind, data) VALUES (:itime, :ikind, :idata)", array('itime' => $date, 'ikind' => $data[0], 'idata' => (int)$data[1])); break;
				default:
			}
		}
    }
?>