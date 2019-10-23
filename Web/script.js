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