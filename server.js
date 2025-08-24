
async function connectToServer(){
    const details = await fetch('http://10.201.123.183:8000', {mode:'no-cors'});

    return details;
}

async function mainConnection(){
    try {
        const details = await connectToServer();
        console.log("details" , details);
    } catch (error) {
        console.log("error",error)
    }
}

mainConnection()