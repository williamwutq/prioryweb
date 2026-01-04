
function showToast(text) {
    const toast = document.getElementById("toast");
    toast.classList.add("show");
    toast.innerText = text;
    setTimeout(() => {
        toast.classList.remove("show");
        toast.innerText = "";
    }, 3000);
}

window.onload = showToast("This is a website in development");