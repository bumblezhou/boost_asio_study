function add_test_element() {
    var test_element = document.createElement("div");
    test_element.innerHTML = "This is a test dev element";
    test_element.style = "background: lightgreen;";
    document.getElementsByTagName("body")[0].appendChild(test_element);
}