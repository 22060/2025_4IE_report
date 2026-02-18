//スマホ判定関数
function isSmartPhone() {
  if (document.body.offsetWidth <= 800 || navigator.userAgent.match(/iPhone|Android.+Mobile/)) {
    return true;
  } else {
    return false;
  }
}
//スマホだった場合には，ハンバーガーメニューを表示させタイトルの横幅を小さくする．
if(isSmartPhone()){
    document.getElementById("titles").style.display = "none";
}else{
    document.getElementById("hamburger").style.display = "none";
}