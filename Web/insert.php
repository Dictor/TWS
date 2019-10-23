<?php
    namespace Dictor\OSAM2019;
    require_once('lib/db.php');

    if (!array_key_exists('TIME', $_GET) || !array_key_exists('KIND', $_GET) || !array_key_exists('DATA', $_GET)) {
        echo 'INVALID_PARAM';
        exit();
    }

    if (Model::UseDB() != 0) {
        echo 'DB_ERROR';
        exit();
    }

    if (InsertData::Insert($_GET['TIME'], $_GET['KIND'], $_GET['DATA'])) {
        echo 'OK';
    } else {
        echo 'UNKNOWN_KIND';
    }

    Class InsertData {
        public static function Insert(string $time, string $kind, string $data) {
            if ($kind == 'LOG' || $kind == 'EVENT' || $kind == 'SENSOR') {
                Model::AddRow($time, $kind, explode(',', $data));
                return TRUE;
            } else {
                return FALSE;
            } 
        }
    }
?>