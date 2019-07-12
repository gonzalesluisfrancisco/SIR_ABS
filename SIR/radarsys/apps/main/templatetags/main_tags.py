from django.template.defaulttags import register
from django.utils.safestring import mark_safe

@register.filter
def attr(instance, key):
    
    display_key = "get_" + key + "_display"
    
    if key=='name':
        return '{}'.format(instance)
    
    if hasattr(instance, display_key):
        return getattr(instance, display_key)()
    
    if hasattr(instance, key):
        return getattr(instance, key)
    
    return instance.get(key)

@register.filter
def title(s):
    return s.split('__')[-1].replace('_', ' ').title()

@register.filter
def value(instance, key):
    
    item = instance
    if key=='name':
        return '%s' % item
    for my_key in key.split("__"):
        item = attr(item, my_key)
    
    return item

@register.simple_tag
def get_verbose_field_name(instance, field_name):
    """
    Returns verbose_name for a field.
    """
    if field_name=='ipp_unit':
        return 'IPP [Km(Units)]'
    return mark_safe(instance._meta.get_field(field_name).verbose_name)