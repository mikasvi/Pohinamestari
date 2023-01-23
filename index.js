// testataan git versiohistoria part1
const { SerialPort } = require('serialport') //tuodaan kirjastot
const { ReadlineParser } = require('@serialport/parser-readline')
const schedule = require('node-schedule');
const fs = require('fs')
const { exec } = require('child_process'); //tuodaan kirjasto käyttöön, jolla voi suorittaa esim windows komentorivikomentoja


const port = new SerialPort({path: 'COM3', baudRate: 9600}); //alustetaan portti, valitaan nopeus
const parser = port.pipe(new ReadlineParser({ delimiter: '\r\n' }))  //kytketään parseri eli rivivaihto

parser.on('data', (row) => {  //kuuntelija -> suorita parametri
    const now = new Date() //tämänhetkinen aika
    const fileName = `${now.getFullYear()}-${now.getMonth()+1}-${now.getDate()}-${now.getHours()}.txt` // tiedostonimi, korjattu kuukausi +1 merkinnällä
    fs.appendFileSync(fileName, `${row}\n`) //luo
})
function generatehtml(){
    let date = new Date()
    let html = ''
    for (let i = 0; i < 10; i++) {
        date.setHours(date.getHours()-1)
        const fileName = `${date.getFullYear()}-${date.getMonth()+1}-${date.getDate()}-${date.getHours()}.txt`
        html += `<a href="${fileName}">${fileName}</a><br>\n`
    }
    fs.writeFileSync('index.html', html)
    exec('az storage blob upload --overwrite --account-name csb10032001aa52bc94 ' +
        '--account-key tyRDhm1thOARRT893oLo7EgCzn+jL/Pln2NAnczirg7Iq8WLFkJqgxn35x9d8wlQIbfDYLH7sCNNAWahuSLiwA== ' +
        `--container-name $web --name index.html --file index.html`,
        error => {
            if (error) {
                console.log(error)
            }}) // tarkistaa virheet, ei tulost jos ok
}

const job = schedule.scheduleJob('0 * * * *', function() { // Suoritetaan aina, kun minuutit on nolla eli kerran tunnissa 0 * * * *
    console.log('Uploading...')
    let date = new Date() //tämänhetkinen aika
    date.setHours(date.getHours()-1)
   // date.setMinutes(date.getMinutes()-1) voi käyttää halutessaan minuutteja
    generatehtml()
    const fileName = `${date.getFullYear()}-${date.getMonth()+1}-${date.getDate()}-${date.getHours()}.txt` // tiedostonimi, korjattu kuukausi +1 merkinnällä
    exec('az storage blob upload --account-name csb10032001aa52bc94 ' +
        '--account-key tyRDhm1thOARRT893oLo7EgCzn+jL/Pln2NAnczirg7Iq8WLFkJqgxn35x9d8wlQIbfDYLH7sCNNAWahuSLiwA== ' +
        `--container-name $web --name ${fileName} --file ${fileName}`,
            error => {
                if (error) {
                    console.log(error)
                }}) // tarkistaa virheet, ei tulosta "null" jos ok

});


