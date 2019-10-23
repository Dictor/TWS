function showPage(num) {
    var pageList = Array("page-dashboard", "page-log");
    for (var i = 0; i < pageList.length; i++) {
        if (i == num) {
            $("#" + pageList[num]).removeClass("page-div-hide");
            $("#" + pageList[num]).addClass("page-div-show");
        } else {
            $("#" + pageList[i]).removeClass("page-div-show");
            $("#" + pageList[i]).addClass("page-div-hide");
        }
    } 
}

function ProcessUI() {
    showPage(0);
    setTempHumiCard();
    setSystemCard();
    setEventCard();

    Highcharts.chart('graph-temphumi', {
        chart: {type: 'line'},
        title: {text: ''},
        xAxis: {categories: TimeToSince(ExtractKeyArray(jsSen, "time", 30))},
        yAxis: {title: {text: ''}},
        plotOptions: {line: {dataLabels: {enabled: true}, enableMouseTracking: false}},
        series: [{name: '온도', data: ExtractKeyArray(jsSen, "temp", 30)}, 
                {name: '습도', data: ExtractKeyArray(jsSen, "humi", 30)}]
    });

    Highcharts.chart('graph-waterlv', {
        chart: {type: 'line'},
        title: {text: ''},
        xAxis: {categories: TimeToSince(ExtractKeyArray(jsSen, "time", 30))},
        yAxis: {title: {text: ''}},
        plotOptions: {line: {dataLabels: {enabled: true}, enableMouseTracking: false}},
        series: [{name: '수위', data: ExtractKeyArray(jsSen, "water", 30)}]
    });
}

function setTempHumiCard() {
    $("#lastest-temp").text(jsSen[jsSen.length - 1]["temp"] + "'C");
    $("#lastest-humi").text(jsSen[jsSen.length - 1]["humi"] + "%");
}

function setSystemCard() {
    var jsBoot = [];
    var jsPureErr = [];
    jsErr.forEach(function(nowele) {
        if (nowele["data"] == "ERROR,OS_INIT_SUCCESS") {
            jsBoot.push(nowele);
        } else {
            jsPureErr.push(nowele);
        }
    });
    $("#lastest-boot").text(moment(jsBoot[jsBoot.length - 1]["time"]).fromNow() + " 장치 시작");
    $("#lastest-recieve").text(moment(jsSen[jsSen.length - 1]["time"]).fromNow() + " 최종 수신");
    $("#lastest-error").text(jsPureErr.length + " 개의 오류");
    $("#lastest-info").text((jsSen.length + jsEvt.length) + " 개의 데이터");
}

function setEventCard() {
    $("#lastest-event").html("");
    var i = 0, antcnt = 0, cdscnt = 0;
    jsEvt.reverse().forEach(function(nowele) {
        if (i <= 2) document.getElementById("lastest-event").innerHTML += '<li class="list-group-item">' + moment(nowele["time"]).fromNow() + (nowele["kind"] == "CDS" ? " 섬광" : " 전자기장") + " 감지 </li>";
        if (moment(nowele["time"]).diff(moment(), "second") < 600) {
            if (nowele["kind"] == "CDS") {
                cdscnt++;
            } else {
                antcnt++;
            }
        }
        if (cdscnt > 1 && antcnt > 1) {
            $("#lastest-evtresult").text("뇌우 확실");
            $("#lastest-evtresult").css("color","red");
        } else if (cdscnt > 0 || antcnt > 0) {
            $("#lastest-evtresult").text("뇌우 의심");
            $("#lastest-evtresult").css("color","yellow");
        } else {
            $("#lastest-evtresult").text("뇌우 안전");
            $("#lastest-evtresult").css("color","green");
        }
        i++; 
    });

}

function ExtractKeyArray(origin, keyname, limit) {
    var arr = [];
    var i = 0;
    origin.forEach(function(nowele) {
        if (i <= limit) arr.push(nowele[keyname]);
        i++;
    });
    return arr;
}

function TimeToSince(time) {
    var arr = [];
    time.forEach(function(nowele) {
        arr.push(moment(nowele).fromNow());
    });
    return arr;
}