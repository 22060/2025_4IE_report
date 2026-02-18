var xhr = new XMLHttpRequest ();
xhr.onreadystatechange = getinformation;
xhr.open('GET',location.origin + '/materials', true);
xhr.responseType = 'json';
xhr.send(null);

var xhr2 = new XMLHttpRequest ();
xhr2.onreadystatechange = getinformation2;
xhr2.open('GET',location.origin + '/products', true);
xhr2.responseType = 'json';
xhr2.send(null);

var flag = 0;

function getinformation()
{
    if (( xhr. readyState == 4) && (xhr.status == 200))
    {
        a = xhr.response ;
        console.log(a);
        flag += 1;
        if(flag === 2) {
            renderTable();
        }
    }
}

function getinformation2()
{
    if (( xhr2. readyState == 4) && (xhr2.status == 200))
    {
        b = xhr2.response ;
        console.log(b);
        flag += 1;
        if(flag === 2) {
            renderTable();
        }
    }
}
let flag2 = 0;
//二つのエンドポイントに対して非同期処理のため，情報がそろったタイミングで表示関数を発火させる．
function renderTable() {
    s = "<tr><th>製品名</th><th>最大製造単位数</th><th>単位当たり利益(円)</th><th>製造単位数</th><th>必要材料数</th><th>現在在庫</th><th>過不足</th><th>見込み利益</th></tr>";
    for (i = 0 ; i < b.length ; i++)
    {
        console.log(b[i]);
        units = document.getElementById(b[i].id + "prod") ? document.getElementById(b[i].id + "prod").value : 0;
        s+= "<tr>";
        s+= "<td>" + b[i].name + "</td>";
        let dat = JSON.parse(b[i].material);
        let maxUnits = Infinity;
        for (const [key, value] of Object.entries(dat)) {
            let material = a.find(mat => mat.name === key);
            if (material) {
                let possibleUnits = Math.floor((material.unit_value * material.unit_amount) / value);
                if (possibleUnits < maxUnits) {
                    maxUnits = possibleUnits;
                }
            } else {
                maxUnits = 0;
                break;
            }
        }
        s+= "<td>" + maxUnits + "</td>";
        let profitPerUnit = b[i].price*b[i].unit_value - calculateCostPerUnit(dat, a);
        s+= "<td>" + profitPerUnit  + "</td>";
        s+= "<td><input type='number' id='" + b[i].id + "prod' value='"+units +"' min='1' max='" + maxUnits + "''><button onclick='renderTable()'>計算</button></td>";
        s+= "<td>";
        s+="計算単位数: " + units + "<br>";
        for (const [key, value] of Object.entries(dat)) {
            s += key + ": " + fixketa(value * units, a.find(mat => mat.name === key).unit) + "<br>";
        }
        s+= "</td>";
        let currentStockInfo = "";
        for (const [key, value] of Object.entries(dat)) {
            let material = a.find(mat => mat.name === key);
            if (material) {
                currentStockInfo += key + ": " + fixketa(material.unit_amount*material.unit_value, material.unit) + "<br>";
                console.log((material.unit_amount*material.unit_value) + material.unit)
                // currentStockInfo += key + ": " + (material.unit_value * material.unit_amount) + material.unit + "<br>";
            } else {
                currentStockInfo += key + ": 在庫なし<br>";
            }
        }
        s+= "<td>" + currentStockInfo + "</td>";
        let shortageInfo = "";
        for (const [key, value] of Object.entries(dat)) {
            let material = a.find(mat => mat.name === key);
            let requiredAmount = value * units;
            if (material) {
                let availableAmount = material.unit_value * material.unit_amount;
                let val = availableAmount - requiredAmount;
                if (val < 0) {
                    val = -1*val
                    shortageInfo += "<a style='color:red;'>" + key + ": " + fixketa(val, material.unit) + "不足<br></a>";

                    // shortageInfo += "<a style='color:red;'>" + key + ": " + (requiredAmount - availableAmount) + material.unit + "不足<br></a>";
                } else {
                    shortageInfo += key + ": 十分な在庫あり<br>";
                }
            } else {
                shortageInfo += key + ": 在庫なし<br>";
            }
        }
        s+= "<td>" + shortageInfo + "</td>";
        let expectedProfit = profitPerUnit * units;
        s+= "<td>" + fixketa(expectedProfit, "円") + "</td>";
        s+= "</tr>";
    }
    document.getElementById("tables").innerHTML = s;
    t = "<tr><th>合計必要量</th><th>現在在庫</th><th>合計過不足</th></tr>";
    let totalNeeded = {};
    for (i = 0 ; i < b.length ; i++)
    {
        units = document.getElementById(b[i].id + "prod") ? document.getElementById(b[i].id + "prod").value : 0;
        let dat = JSON.parse(b[i].material);
        for (const [key, value] of Object.entries(dat)) {
            if (!(key in totalNeeded)) {
                totalNeeded[key] = 0;
            }
            totalNeeded[key] += value * units;
        }
    }
    let totalStockInfo = "";
    let totalShortageInfo = "";
    for (const [key, value] of Object.entries(totalNeeded)) {
        totalStockInfo += key + ": ";
        totalShortageInfo += key + ": ";
        let material = a.find(mat => mat.name === key);
        if (material) {
            let availableAmount = material.unit_value * material.unit_amount;
            totalStockInfo += fixketa(availableAmount, material.unit) + "<br>";
            let val = availableAmount - value;
            if (val < 0) {
                val = -1*val
                totalShortageInfo += "<a style='color:red;'>" + fixketa(val, material.unit) + "不足<br></a>";
                // totalShortageInfo += "<a style='color:red;'>" + (value - availableAmount) + material.unit + "不足<br></a>";
            } else {
                totalShortageInfo += "十分な在庫あり<br>";
            }
        } else {
            totalStockInfo += "在庫なし<br>";
            totalShortageInfo += "<a style='color:red;'>" + value + "不足<br></a>";
        }
    }
    t+= "<tr>";
    t+= "<td>" + Object.entries(totalNeeded).map(([key, value]) => key + ": " + fixketa(value, a.find(mat => mat.name === key).unit)).join("<br>") + "</td>";
    t+= "<td>" + totalStockInfo + "</td>";
    t+= "<td>" + totalShortageInfo + "</td>";
    t+= "</tr>";
    document.getElementById("all").innerHTML = t;
}

function calculateCostPerUnit(materialsNeeded, allMaterials) {
    let totalCost = 0;
    for (const [key, value] of Object.entries(materialsNeeded)) {
        let material = allMaterials.find(mat => mat.name === key);
        if (material) {
            totalCost += (material.price / material.unit_value) * value;
            console.log("材料:" + key + " 単価:" + (material.price / material.unit_value) + " 必要量:" + value + " 費用:" + ((material.price / material.unit_value) * value) + " totalCost:" + totalCost);
        }
    }
    console.log("合計費用:" + totalCost);
    return totalCost;
}


function productionStatus()
{
    if (( xhr2. readyState == 4) && (xhr2.status == 200))
    {
        console.log("製造処理が完了しました");
        // alert("製造処理が完了しました");
        location.reload();
    }
}

function orgRound(value, base) {
    return Math.round(value * base) / base;
}

function fixketa(value,unit){
    if(unit === "g" || unit === "L"){
        if(value >= 1000 && value < 1000000){
            return orgRound(value / 1000, 100) + "k" + unit;
        }else if(value >= 1000000){
            return orgRound(value / 1000000, 100) + "t" + unit;
        }
        if(value < 1){
            return orgRound(value * 1000, 100) + "m" + unit;
        }
    }else if(unit === "個"){
        if(value >= 10000){
            return Math.round(value / 10000) + "万個";
        }
        return Math.round(value) + unit;
    }else if(unit === "円"){
        if(value >= 10000 && value < 100000000){
            return orgRound(value / 10000, 100) + "万円";
        }else if(value >= 100000000){
            return orgRound(value / 100000000, 100) + "億円";
        }
        return Math.round(value) + unit;
    }
    return orgRound(value, 100) + unit;
}