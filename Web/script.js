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
        xAxis: {categories: TimeToSince(ExtractKeyArray(jsSen, "time"))},
        yAxis: {title: {text: ''}},
        plotOptions: {line: {dataLabels: {enabled: true}, enableMouseTracking: false}},
        series: [{name: '온도', data: ExtractKeyArray(jsSen, "temp")}, 
                {name: '습도', data: ExtractKeyArray(jsSen, "humi")}]
    });

    Highcharts.chart('graph-waterlv', {
        chart: {type: 'line'},
        title: {text: ''},
        xAxis: {categories: TimeToSince(ExtractKeyArray(jsSen, "time"))},
        yAxis: {title: {text: ''}},
        plotOptions: {line: {dataLabels: {enabled: true}, enableMouseTracking: false}},
        series: [{name: '수위', data: ExtractKeyArray(jsSen, "water")}]
    });
}

function setTempHumiCard() {
    $("#lastest-temp").text(jsSen[jsSen.length - 1]["temp"] + "'C");
    $("#lastest-humi").text(jsSen[jsSen.length - 1]["humi"] + "%");
}

function setSystemCard() {
    $("#lastest-boot").text(moment(jsBoot[jsBoot.length - 1]["time"]).fromNow() + " 장치 시작");
    $("#lastest-recieve").text(moment(jsSen[jsSen.length - 1]["time"]).fromNow() + " 최종 수신");
}

function setEventCard() {
    $("#lastest-event").html("");
    jsEvt.forEach(function(nowele) {
        document.getElementById("lastest-event").innerHTML += '<li class="list-group-item">' + moment(nowele["time"]).fromNow() + (nowele["time"] == "CDS" ? " 섬광" : " 전자기장") + " 감지 </li>";
    });
}

function ExtractKeyArray(origin, keyname) {
    var arr = [];
    origin.forEach(function(nowele) {
        arr.push(nowele[keyname]);
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