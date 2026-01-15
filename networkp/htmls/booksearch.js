var xhr = new XMLHttpRequest ();

 function search ()
 {
 t = document . getElementById ("keyword"). value;
 num = document.getElementById("num").value;
 offset = document.getElementById("offset").value;

 xhr. onreadystatechange = checkStatus ;
 xhr.open('GET', 'https://www.googleapis.com/books/v1/volumes?q=' + t + '&maxResults=' + num + '&startIndex=' + offset, true);
 xhr. responseType = 'json';
 xhr.send(null);
 }

 function checkStatus ()
{
s = "";

if (( xhr. readyState == 4) && (xhr.status == 200))
{
a = xhr.response ;

s = "全部で" + a. totalItems + "件あります。<p>";
s = s + "<table border =1>";
s = s + "<tr><th>Rating</th><th>タイトル</th><th>著者</th><th>出版社</th><th>ページ数</th><th>発行日</th><th>あらすじ</th></tr>";
for(i= 0 ; i < a.items.length ; i++)
{
s = s + "<tr >";
s = s + "<td >" + a.items[i]. volumeInfo . averageRating + " </td >";
s = s + "<td >" + a.items[i]. volumeInfo .title + " </td >";
if (a.items[i]. volumeInfo .authors == undefined)
{
s = s + "<td >??? </td >";
}
else
{
numOfAuthors = a.items[i]. volumeInfo .authors.length;
s = s + "<td >";
for (j = 0 ; j < numOfAuthors ; j++)
{
s = s + a.items[i]. volumeInfo .authors[j] + " ";
}
s = s + " </td >";
}

s = s + "<td >" + a.items[i]. volumeInfo . publisher + " </td >";
s = s + "<td >" + a.items[i]. volumeInfo . pageCount + " </td >";
s = s + "<td >" + a.items[i]. volumeInfo . publishedDate + " </td >";
s = s + "<td >" + a.items[i]. volumeInfo . description + " </td >";
s = s + " </tr >";
}
s = s + " </table >";
}
document. getElementById ("results"). innerHTML = s;
}