var xhr = new XMLHttpRequest ();
xhr.onreadystatechange = getinformation;
xhr.open('GET',location.origin + '/materials', true);
xhr.responseType = 'json';
xhr.send(null);

function getinformation()
{
    if (( xhr. readyState == 4) && (xhr.status == 200))
    {
        a = xhr.response ;
        console.log(a);
        l = a.length;
        s = "<tr><th>材料名</th><th>在庫量</th><th>単位</th><th>在庫量</th></tr>";
        for (i = 0 ; i < l ; i++)
        {
            s+= "<tr>";
            s+= "<td>" + a[i].name + "</td>";
            s+= "<td>" + a[i].unit_amount + "</td>";
            if(a[i].unit_value >= 1000 && a[i].unit === "g") {
                let val = a[i].unit_value / 1000;
                s+= "<td>" + val + "kg</td>";
            }else {
                s+= "<td>" + a[i].unit_value + a[i].unit + "</td>";
            }
            if( a[i].unit_value * a[i].unit_amount >= 1000 && a[i].unit === "g") {
                let val = (a[i].unit_value * a[i].unit_amount) / 1000;
                s+= "<td>" + val + "kg</td>";
            }else {
                s+= "<td>" + (a[i].unit_value * a[i].unit_amount) + a[i].unit + "</td>";
            }
            s+= "</tr>";
        }
        document.getElementById("tables").innerHTML = s;
        // セレクター使用してinputfieldに入力欄を追加(input type='select')
        let inputfield = document.getElementById("inputfield");
        let formcontent = "<select name='field' id='materialSelect'><option value=''>材料を選択してください</option>";
        for (i = 0 ; i < l ; i++)
        {
            formcontent += "<option value='" + a[i].id + "'>" + a[i].name + "</option>";
        }
        formcontent += "</select>";
        formcontent += " 入荷量: <input type='number' id='amount' min='0' value='0'> ";
        formcontent += "<button type='button' onclick='doArrival(document.getElementById(\"materialSelect\").value)'>入荷処理</button>";
        inputfield.innerHTML = formcontent;

    }
}

var xhr2 = new XMLHttpRequest ();
function doArrival(id) {
    if(id === "") {
        alert("材料を選択してください");
        return;
    }
    console.log("入荷処理が実行されました");
    const amount = document.getElementById("amount").value;
    xhr2.onreadystatechange = arrivalStatus;
    xhr2.open('POST',location.origin + '/Arrival', true);
    xhr2.responseType = 'json';
    xhr2.setRequestHeader('Content-Type', 'application/json');
    xhr2.send(JSON.stringify({id: id, quantity: amount}));
}

function arrivalStatus()
{
    if (( xhr2. readyState == 4) && (xhr2.status == 200))
    {
        console.log("入荷処理が完了しました");
        // alert("入荷処理が完了しました");
        location.reload();
    }
}