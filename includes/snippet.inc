<?php

/**
 * @file
 * Contains the JavaScript snippet insertion code.
 */

/**
 * Returns JavaScript snippets.
 *
 * @return array
 *   Associative array of snippets keyed by type: script, noscript and
 *   data_layer.
 */
function google_tag_snippets() {
  $snippets = array(
    'script' => _google_tag_script_snippet(),
    'noscript' => _google_tag_noscript_snippet(),
    'data_layer' => _google_tag_data_layer_snippet(),
  );
  // Allow other modules to alter the snippets.
  backdrop_alter('google_tag_snippets', $snippets);
  return $snippets;
}

/**
 * Returns JavaScript script snippet.
 *
 * @return array
 *   The script snippet.
 */
function _google_tag_script_snippet() {
  // Gather data.
  $container_id = _google_tag_variable_clean('google_tag_container_id');
  $data_layer = _google_tag_variable_clean('google_tag_data_layer', 'dataLayer');
  $query = _google_tag_environment_query();
  $compact = config_get('google_tag.settings','google_tag_compact_snippet');

  // Build script snippet.
  $script = <<<EOS
(function(w,d,s,l,i){

  w[l]=w[l]||[];
  w[l].push({'gtm.start':new Date().getTime(),event:'gtm.js'});
  var f=d.getElementsByTagName(s)[0];
  var j=d.createElement(s);
  var dl=l!='dataLayer'?'&l='+l:'';
  j.type='text/javascript';
  j.src='https://www.googletagmanager.com/gtm.js?id='+i+dl+'$query';
  j.async=true;
  f.parentNode.insertBefore(j,f);

})(window,document,'script','$data_layer','$container_id');
EOS;
  if ($compact) {
    $script = str_replace(array("\n", '  '), '', $script);
  }
/*
  $script = <<<EOS
<!-- Google Tag Manager -->
$script
<!-- End Google Tag Manager -->
EOS;
*/
  return $script;
}

/**
 * Returns JavaScript noscript snippet.
 *
 * @return array
 *   The noscript snippet.
 */
function _google_tag_noscript_snippet() {
  // Gather data.
  $container_id = _google_tag_variable_clean('google_tag_container_id');
  $query = _google_tag_environment_query();
  $compact = config_get('google_tag.settings','google_tag_compact_snippet');

  // Build noscript snippet.
  $noscript = <<<EOS
<noscript aria-hidden="true"><iframe src="https://www.googletagmanager.com/ns.html?id=$container_id$query"
 height="0" width="0" style="display:none;visibility:hidden"></iframe></noscript>
EOS;
  if ($compact) {
    $noscript = str_replace("\n", '', $noscript);
  }
/*
  $noscript = <<<EOS
<!-- Google Tag Manager -->
$noscript
<!-- End Google Tag Manager -->
EOS;
*/
  return $noscript;
}

/**
 * Returns JavaScript data layer snippet or adds items to data layer.
 *
 * @return string|null
 *   The data layer snippet or NULL.
 */
function _google_tag_data_layer_snippet(&$classes = array()) {
  // Gather data.
  $data_layer = _google_tag_variable_clean('google_tag_data_layer', 'dataLayer');
  $whitelist = config_get('google_tag.settings','google_tag_whitelist_classes');
  $blacklist = config_get('google_tag.settings','google_tag_blacklist_classes');

  $classes = array();
  $names = array('whitelist', 'blacklist');
  foreach ($names as $name) {
    google_tag_text_clean($$name, 'array');
    if (empty($$name)) {
      continue;
    }
    $classes["gtm.$name"] = $$name;
  }

  // Build data layer snippet.
  $script = $classes ? "var $data_layer = [" . backdrop_json_encode($classes) . '];' : '';
  return $script;
}

/**
 * Returns a query string with the environment parameters.
 *
 * @return string
 *   The query string.
 */
function _google_tag_environment_query() {
  if (config_get('google_tag.settings','google_tag_include_environment') == 0) {
    return '';
  }

  // Gather data.
  $environment_id = _google_tag_variable_clean('google_tag_environment_id');
  $environment_token = _google_tag_variable_clean('google_tag_environment_token');

  // Build query string.
  return "&gtm_auth=$environment_token&gtm_preview=$environment_id&gtm_cookies_win=x";
}

/**
 * Returns a cleansed variable.
 *
 * @param string $variable
 *   The variable name.
 *
 * @return string
 *   The cleansed variable.
 */
function _google_tag_variable_clean($variable) {
  return trim(backdrop_json_encode(config_get('google_tag.settings',$variable)), '"');
}
