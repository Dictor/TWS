<?php
    namespace Dictor\OSAM2019;
    require_once('lib/db.php');
?>
<html>
	<head>
		<meta charset="utf-8">
		<meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
		<title>TWS Dashboard</title>
    
        <!--Custom font-->
		<link href="https://fonts.googleapis.com/earlyaccess/nanumgothic.css" rel="stylesheet" type="text/css">
		<link href="https://cdnjs.cloudflare.com/ajax/libs/simple-line-icons/2.4.1/css/simple-line-icons.min.css" rel="stylesheet">
					
		<!--BootStrap-->
		<link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css" integrity="sha384-ggOyR0iXCbMQv3Xipma34MD+dH/1fQ784/j6cY/iJTQUOhcWr7x9JvoRxT2MZw1T" crossorigin="anonymous">
	    <script src="https://code.jquery.com/jquery-3.3.1.slim.min.js" integrity="sha384-q8i/X+965DzO0rT7abK41JStQIAqVgRVzpbzo5smXKp4YfRvH+8abtTE1Pi6jizo" crossorigin="anonymous"></script>
		<script src="https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.14.7/umd/popper.min.js" integrity="sha384-UO2eT0CpHqdSJQ6hJty5KVphtPhzWj9WO1clHTMGa3JDZwrnQq4sF86dIHNDz0W1" crossorigin="anonymous"></script>
		<script src="https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/js/bootstrap.min.js" integrity="sha384-JjSmVgyd0p3pXB1rRibZUAYoIIy6OrQ6VrjIEaFf/nJGzIxFDsf4x0xIM+B07jRM" crossorigin="anonymous"></script>
	</head> 	
    <body>
        <script>
            <?php 
                if (Model::UseDB() != 0) {
                    echo 'alert("DB ERROR!");';
                    exit();
                }
                echo 'var jsLog = '.json_encode(Model::GetLogs(-1)).";\n";
                echo '            var jsEvt = '.json_encode(Model::GetEvents(-1)).";\n";
                echo '            var jsSen = '.json_encode(Model::GetSensorData(-1)).";\n";
            ?>
        </script>   
        <nav class="navbar navbar-expand-lg navbar-dark bg-dark">
            <a class="navbar-brand" href="#">TWS</a>
            <button class="navbar-toggler" type="button" data-toggle="collapse" data-target="#navbarSupportedContent" aria-controls="navbarSupportedContent" aria-expanded="false" aria-label="Toggle navigation">
                <span class="navbar-toggler-icon"></span>
            </button>
            <div class="collapse navbar-collapse" id="navbarSupportedContent">
                <ul class="navbar-nav mr-auto">
                <li class="nav-item active">
                    <a class="nav-link" href="#">대시보드 <span class="sr-only">(current)</span></a>
                </li>
                <li class="nav-item">
                    <a class="nav-link" href="#">로그</a>
                </li>
            </div>
        </nav>
        <div id="page-dashboard"></div>
        <div id="page-log">
            <table class="table">
                <thead class="thead-dark">
                    <tr>
                        <th scope="col">시간</th>
                        <th scope="col">내용</th>
                    </tr>
                </thead>
                <tbody>
                    <?php 
                        foreach(Model::GetLogs(-1) as $nowlog) {
                            echo '<tr><th scope="row">'.$nowlog['time'].'</th><td>'.$nowlog['data'].'</td></tr>';
                        }
                    ?>
                </tbody>
            </table>    
        </div>
    </body>
</html>