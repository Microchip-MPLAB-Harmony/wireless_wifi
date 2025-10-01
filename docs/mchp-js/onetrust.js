(function(){
  var btnToggle = document.querySelector('.show-cookie-settings');
  if (btnToggle) {
    btnToggle.addEventListener('click', function() {
      if (typeof OneTrust === 'undefined') {
        console.error('OneTrust object is not defined.');
        return;
      }
      OneTrust.ToggleInfoDisplay();
    });
  }
})();
