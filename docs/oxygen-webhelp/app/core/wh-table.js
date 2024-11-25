define(['jquery'], function ($) {
  $(document).ready(function () {
    $('table').each(function () {
      var firstHeaderHeight = $(this).find('thead tr:first').outerHeight();
      $(this)
        .find('thead tr:nth-child(2) th')
        .css('top', firstHeaderHeight + 'px');
    });
  });
});
