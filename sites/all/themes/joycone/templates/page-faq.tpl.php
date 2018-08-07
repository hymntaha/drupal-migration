<?php include 'page__header.tpl.php'; ?>
<div id="faqs">
    <div id="faq_frames" class="lfloat tcenter">
        <?php echo taxonomy_render_nodes(nodeorder_select_nodes(array(JOYCONE_SIDEBAR_TAXONOMY_FAQ), 'or', 0, false, 'n.sticky DESC, tn.weight_in_tid')); ?>
    </div>
    <div id="faq_body" class="accordion">
        <?php echo $content; ?>
    </div>
    <div id="faq_contact" class="tright ">
        <span>Still have a question?</span>
        <a href="/contact-us" class="button-round">Contact Us &#9654;</a>
    </div>
</div>
<?php include 'page__footer.tpl.php'; ?>
