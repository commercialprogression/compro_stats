<?php
/**
 * @file
 * Code for the block content of Compro Stats and Graphs.
 */

/**
 * Gather # of Visits per day from the past week from GA and arrange it
 * in a line chart
 */
function _compro_stats_site_visits() {
  chart_get_library('google');
  $params = array(
    'metrics' => array('ga:visits'),
    'dimensions' => array('ga:date'),
    'sort_metric' => array('ga:date'),
    'start_date' => strtotime('-8 days'),
    'end_date' => strtotime('-1 day'),
  );
  $feed = google_analytics_api_report_data($params);
  if ($feed->error) {
    if ($feed->error == 'Code: 401 - Error: Unauthorized - Message: GDatarequiredAuthorizationLogin Required') {
      return t('Please go to the ') . l(t('Google Analytics Report'), 'admin/config/system/google-analytics-reports') .
             t(' page and setup GA access to view this chart');
    }
    return FALSE;
  }
  $max_visits = 0;
  $data = array();
  foreach ($feed->results as $row) {
    $data[] = $row['visits'];
    $max_visits = max($row['visits'], $max_visits);
  }
  $chart = array(
    '#title' => '',
    '#chart_library' => 'google',
    '#chart_id' => 'compro_site_visits',
    '#chart_type' => 'line',
    '#type' => 'chart',
  );
  $chart['views'] = array(
    '#type' => 'chart_data',
    '#title' => t('Sessions'),
    '#data' => $data,
  );
  $last_day = end($feed->results);
  $title = t('The most visits on a single day was @max.  Yesterday there were @yesterday visits.', array(
    '@max' => $max_visits,
    '@yesterday' => $last_day['visits']
  ));
  $chart['#attributes'] = array(
    'title' => $title,
    'style' => 'height:200px; width:100%'
  );
  return drupal_render($chart);
}

/**
 * Gather various statistics (total visits, bounce rate, # page views, avg time on site,
 * # pages per visit, % new users) from the past week from GA and arrange it
 * in a table
 */
function _compro_stats_site_usage() {
  chart_get_library('google');
  $params = array(
    'metrics' => array(
      'ga:visits',
      'ga:bounces',
      'ga:entrances',
      'ga:pageviews',
      'ga:timeOnSite',
      'ga:newVisits'
    ),
    'start_date' => strtotime('-8 days'),
    'end_date' => strtotime('-1 day'),
  );
  $feed = google_analytics_api_report_data($params);
  if ($feed->error) {
    if ($feed->error == 'Code: 401 - Error: Unauthorized - Message: GDatarequiredAuthorizationLogin Required') {
      return t('Please go to the ') . l(t('Google Analytics Report'), 'admin/config/system/google-analytics-reports') .
             t(' page and setup GA access to view this chart');
    }
    return FALSE;
  }

  $results = $feed->results[0];

  if ($results['visits'] <= 0) {
    $results['pages_per_visit'] = '0.00';
    $results['new_visits'] = '0.00%';
    $results['time_on_site'] = '0:00';
  }
  else {
    $results['pages_per_visit'] = number_format(round($results['pageviews'] / $results['visits'], 2), 2);
    $results['new_visits'] = number_format(round(($results['newVisits'] / $results['visits']) * 100, 2), 2) . '%';
    $results['time_on_site'] = format_interval($results['timeOnSite'] / $results['visits']);
  }

  if ($results['entrances'] <= 0) {
    $results['bounces'] = '0.00%';
  }
  else {
    $results['bounces'] = number_format(round(($results['bounces'] / $results['entrances']) * 100, 2), 2) . '%';
  }

  $results['entrances'] = number_format($results['entrances']);
  $results['pageviews'] = number_format($results['pageviews']);

  $data = array(
    array('Visits', $results['entrances'], 'Bounce Rate', $results['bounces']),
    array(
      'Page Views',
      $results['pageviews'],
      'Avg. Time on Site',
      $results['time_on_site']
    ),
    array(
      'Pages/Visit',
      $results['pages_per_visit'],
      'New Visits',
      $results['new_visits']
    )
  );
  return theme('table', array('rows' => $data));
}

/**
 * Gather list of Top 5 Pages from the past week from GA and arrange it
 * in a bar chart
 */
function _compro_stats_top_pages() {
  chart_get_library('google');
  $params = array(
    'metrics' => array('ga:pageviews'),
    'dimensions' => array('ga:pageTitle', 'ga:hostname', 'ga:pagePath'),
    'sort_metric' => array('-ga:pageviews'),
    'start_date' => strtotime('-8 days'),
    'end_date' => strtotime('-1 day'),
    'sort' => '-ga:pageviews',
    'max_results' => 5,
  );
  $feed = google_analytics_api_report_data($params);
  if ($feed->error) {
    if ($feed->error == 'Code: 401 - Error: Unauthorized - Message: GDatarequiredAuthorizationLogin Required') {
      return t('Please go to the ') . l(t('Google Analytics Report'), 'admin/config/system/google-analytics-reports') .
             t(' page and setup GA access to view this chart');
    }
    return FALSE;
  }
  $chart = array(
    '#title' => '',
    '#chart_library' => 'google',
    '#chart_id' => 'compro_stats_top_pages',
    '#chart_type' => 'bar',
    '#type' => 'chart',
    '#legend_position' => 'none',
  );
  $pageviews = array();
  $pages = array();
  foreach ($feed->results as $page) {
    $pages[] = $page['pageTitle'];
    $pageviews[] = (int) number_format($page['pageviews']);
  }
  $chart['views'] = array(
    '#type' => 'chart_data',
    '#title' => $pageviews,
    '#data' => $pageviews,
  );
  $chart['xaxis'] = array(
    '#type' => 'chart_xaxis',
    '#labels' => $pages,
  );
  $chart['#attributes'] = array(
    'title' => 'Top Pages',
    'style' => 'height:200px; width:100%'
  );
  return drupal_render($chart);
}

/**
 * Gather list of Top 5 Referrals from the past week from GA and arrange it
 * in a bar chart
 */
function _compro_stats_top_referrals() {
  chart_get_library('google');
  $params = array(
    'metrics' => array('ga:visits'),
    'dimensions' => array('ga:source', 'ga:medium'),
    'sort_metric' => array('-ga:visits'),
    'filters' => 'ga:medium==referral',
    'start_date' => strtotime('-8 days'),
    'end_date' => strtotime('-1 day'),
    'max_results' => 5,
  );
  $feed = google_analytics_api_report_data($params);
  if ($feed->error) {
    if ($feed->error == 'Code: 401 - Error: Unauthorized - Message: GDatarequiredAuthorizationLogin Required') {
      return t('Please go to the ') . l(t('Google Analytics Report'), 'admin/config/system/google-analytics-reports') .
             t(' page and setup GA access to view this chart');
    }
    return FALSE;
  }
  $chart = array(
    '#title' => '',
    '#chart_library' => 'google',
    '#chart_id' => 'compro_stats_top_referrals',
    '#chart_type' => 'bar',
    '#type' => 'chart',
    '#legend_position' => 'none',
  );
  $pageviews = array();
  $referrers = array();
  foreach ($feed->results as $page) {
    $referrers[] = $page['source'];
    $pageviews[] = (int) number_format($page['visits']);
  }
  $chart['views'] = array(
    '#type' => 'chart_data',
    '#title' => $pageviews,
    '#data' => $pageviews,
  );
  $chart['xaxis'] = array(
    '#type' => 'chart_xaxis',
    '#labels' => $referrers,
  );
  $chart['#attributes'] = array(
    'title' => 'Top Referrals',
    'style' => 'height:200px; width:100%'
  );
  return drupal_render($chart);
}

/**
 * Gather list of Browsers by Session from the past week from GA and arrange it
 * in a bar chart
 */
function _compro_stats_browsers() {
  chart_get_library('google');
  $params = array(
    'metrics' => array('ga:sessions'),
    'dimensions' => array('ga:browser'),
    'sort_metric' => array('-ga:sessions'),
    'start_date' => strtotime('-8 days'),
    'end_date' => strtotime('-1 day'),
  );
  $feed = google_analytics_api_report_data($params);
  if ($feed->error) {
    if ($feed->error == 'Code: 401 - Error: Unauthorized - Message: GDatarequiredAuthorizationLogin Required') {
      return t('Please go to the ') . l(t('Google Analytics Report'), 'admin/config/system/google-analytics-reports') .
             t(' page and setup GA access to view this chart');
    }
    return FALSE;
  }
  $chart = array(
    '#title' => '',
    '#chart_library' => 'google',
    '#chart_id' => 'compro_stats_browsers',
    '#chart_type' => 'bar',
    '#type' => 'chart',
    '#legend_position' => 'none',
  );
  $sessions = array();
  $browsers = array();
  foreach ($feed->results as $page) {
    $browsers[] = $page['browser'];
    $sessions[] = (int) number_format($page['sessions']);
  }
  $chart['views'] = array(
    '#type' => 'chart_data',
    '#title' => $sessions,
    '#data' => $sessions,
  );
  $chart['xaxis'] = array(
    '#type' => 'chart_xaxis',
    '#labels' => $browsers,
  );
  $chart['#attributes'] = array(
    'title' => 'Browsers',
    'style' => 'height:200px; width:100%'
  );
  return drupal_render($chart);
}