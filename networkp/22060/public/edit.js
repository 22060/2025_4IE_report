var xhr = new XMLHttpRequest ();
xhr.onreadystatechange = getinformation;
xhr.open('GET',location.origin + '/products', true);
xhr.responseType = 'json';
xhr.send(null);
console.log(location)
let a;
function getinformation()
{
    if (( xhr. readyState == 4) && (xhr.status == 200))
    {
        a = xhr.response ;
        l = a.length;
        //スマホかどうかの確認を行い，スマホだった場合には情報を盾に伸ばして文字のサイズを大きくする．
        if(isSmartPhone()){
            s = ""
            for (i = 0 ; i < l ; i++){
                s+= "<div class='container'><label style='display:flex;'><h1 style='font-size: 64px;'>" + a[i].name + "</h1>";
                s+= "<p style='margin: 48px 0px 0px;font-size:32px'>単位量: " + a[i].unit_value + a[i].unit + "</p></label>";
                s+="<p style='font-size:32px'>材料:</p><form>" ;
                let dat = JSON.parse(a[i].material);
                s += dat.小麦粉 == undefined ? "" : "<div style='display:flex;'><p style='margin: 0px 0px 0px;font-size:32px'>小麦粉</p> : <input type='number' name='" + a[i].id + "小麦粉' value='" + dat.小麦粉 + "'></div>";
                s += dat.卵 == undefined ? "" : "<div style='display:flex;'><p style='margin: 0px 0px 0px;font-size:32px'>卵</p> : <input type='number' name='" + a[i].id + "卵' value='" + dat.卵 + "'></div>";
                s += dat.ベーキングパウダー == undefined ? "" : "<div style='display:flex;'><p style='margin: 0px 0px 0px;font-size:32px'>ベーキングパウダー</p> : <input type='number' name='" + a[i].id + "ベーキングパウダー' value='" + dat.ベーキングパウダー + "'></div>";
                s += dat.砂糖 == undefined ? "" : "<div style='display:flex;'><p style='margin: 0px 0px 0px;font-size:32px'>砂糖</p> : <input type='number' name='" + a[i].id + "砂糖' value='" + dat.砂糖 + "'></div>";
                s += dat.バター == undefined ? "" : "<div style='display:flex;'><p style='margin: 0px 0px 0px;font-size:32px'>バター</p> : <input type='number' name='" + a[i].id + "バター' value='" + dat.バター + "'></div>";
                s += dat.生クリーム == undefined ? "" : "<div style='display:flex;'><p style='margin: 0px 0px 0px;font-size:32px'>生クリーム</p> : <input type='number' name='" + a[i].id + "生クリーム' value='" + dat.生クリーム + "'></div>";
                s += dat.牛乳 == undefined ? "" : "<div style='display:flex;'><p style='margin: 0px 0px 0px;font-size:32px'>牛乳</p> : <input type='number' name='" + a[i].id + "牛乳' value='" + dat.牛乳 + "'></div>";
                s += dat.苺 == undefined ? "" : "<div style='display:flex;'><p style='margin: 0px 0px 0px;font-size:32px'>苺</p> : <input type='number' name='" + a[i].id + "苺' value='" + dat.苺 + "'></div>";
                s += dat.アーモンド == undefined ? "" : "<div style='display:flex;'><p style='margin: 0px 0px 0px;font-size:32px'>アーモンド</p> : <input type='number' name='" + a[i].id + "アーモンド' value='" + dat.アーモンド + "'></div>";
                s += dat.板チョコ == undefined ? "" : "<div style='display:flex;'><p style='margin: 0px 0px 0px;font-size:32px'>板チョコ</p> : <input type='number' name='" + a[i].id + "板チョコ' value='" + dat.板チョコ + "'></div>";
                s += "</form>";
                s+= "<p style='margin: 0px 0px 0px;font-size:32px'>価格(円): " + "<input type='number' id='" + a[i].id + "p' value='" + a[i].price + "'>" + "</p>";
                s += "<button onclick='doEdit(" + a[i].id + ")'>編集</button>";
                s+= "</div>"
            }
        }else{
            s = "<table class='container'><tr><th>商品名</th><th>単位量</th><th>単位</th><th>材料</th><th>価格(円)</th></tr>";
            for (i = 0 ; i < l ; i++)
            {
                console.log(a[i]);
                s+= "<tr>";
                s+= "<td>" + a[i].name + "</td>";
                s+= "<td>" + a[i].unit_value + "</td>";
                s+= "<td>" + a[i].unit + "</td>";
                console.log(a[i].material);
                let dat = JSON.parse(a[i].material);
                s+= "<td><form>" ;
                s += dat.小麦粉 == undefined ? "" : "小麦粉 : <input type='number' name='" + a[i].id + "小麦粉' value='" + dat.小麦粉 + "'> <br>";
                s += dat.卵 == undefined ? "" : "卵 : <input type='number' name='" + a[i].id + "卵' value='" + dat.卵 + "'> <br>";
                s += dat.ベーキングパウダー == undefined ? "" : "ベーキングパウダー : <input type='number' name='" + a[i].id + "ベーキングパウダー' value='" + dat.ベーキングパウダー + "'> <br>";
                s += dat.砂糖 == undefined ? "" : "砂糖 : <input type='number' name='" + a[i].id + "砂糖' value='" + dat.砂糖 + "'> <br>";
                s += dat.バター == undefined ? "" : "バター : <input type='number' name='" + a[i].id + "バター' value='" + dat.バター + "'> <br>";
                s += dat.生クリーム == undefined ? "" : "生クリーム : <input type='number' name='" + a[i].id + "生クリーム' value='" + dat.生クリーム + "'> <br>";
                s += dat.牛乳 == undefined ? "" : "牛乳 : <input type='number' name='" + a[i].id + "牛乳' value='" + dat.牛乳 + "'> <br>";
                s += dat.苺 == undefined ? "" : "苺 : <input type='number' name='" + a[i].id + "苺' value='" + dat.苺 + "'> <br>";
                s += dat.アーモンド == undefined ? "" : "アーモンド : <input type='number' name='" + a[i].id + "アーモンド' value='" + dat.アーモンド + "'> <br>";
                s += dat.板チョコ == undefined ? "" : "板チョコ : <input type='number' name='" + a[i].id + "板チョコ' value='" + dat.板チョコ + "'> <br>";
                s += "</form>";
                s+= "<td>" + "<input type='number' id='" + a[i].id + "p' value='" + a[i].price + "'>" + "</td>";
                s += "<td><button onclick='doEdit(" + a[i].id + ")'>編集</button></td>";
                s+= "</tr>";
            }
            s += "</table>";
        }
        document.getElementById("tables").innerHTML = s;
    }
}

var xhr2 = new XMLHttpRequest ();
function doEdit(id) {
    console.log("編集されました");
    const form = document.querySelector("form input[name='" + id + "小麦粉']");
    const formData = new FormData(form.form);
    let obj = {};
    for (const pair of formData.entries()) {
        const key = pair[0].replace(id, '');
        const value = pair[1];
        if (value !== '') {
            obj[key] = Number(value);
        }
    }
    const jsonString = JSON.stringify({id: id, material: obj, price: Number(document.getElementById(id + "p").value)});
    xhr2.onreadystatechange = getinfo;
    xhr2.open('POST',location.origin + '/edit_product', true);
    xhr2.setRequestHeader('Content-Type', 'application/json');
    console.log(jsonString);
    xhr2.send(jsonString);
}
function getinfo()
{
    // if (( xhr2. readyState == 4) && (xhr2.status == 200))
    // {
    //     console.log(xhr2.response);
    // }
}